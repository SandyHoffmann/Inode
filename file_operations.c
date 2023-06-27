
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "file_operations.h"
#include "super.h"
#include "directory.h"
#include "bitmap.h"

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 16/05/2023.
 * \brief calculate the physical address of data block.
 * \param blockSize = read block
 * \param block = long int representation of block position
 * \return long int physicalPosition
 **/

long int physicalAddress(int blockSize, long int block){
    return block * blockSize + 1;
}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 16/05/2023.
 * \brief Generalizing the search for directorys on memory.
 * \param _fdHd = disk 
 * \param ReadBlock = read block
 * \param block = long int representation of block position
 * \return directory
 **/


Directory* returnDirPhysicalLocation(int fdHd, struct SuperBlock ReadBlock, long int block){
        long int physicalAddressNumber = physicalAddress(ReadBlock.block_size, block);
        if (physicalAddressNumber <= 1){
            physicalAddressNumber = ReadBlock.inode_directory_start;
        }
        // * Catch the father instance
        struct directory *directory = (struct directory *)malloc(ReadBlock.block_size);
        lseek(fdHd, physicalAddressNumber, SEEK_SET);
        read(fdHd, directory, ReadBlock.block_size);
        return directory;

}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 24/04/2023.
 * \update 01/05/2023.
 * \brief Function that allocates a directory in memory, creating the structure based on the absolute path.
 * \param _name The directory's path, starting at /, last word will be taken as folder path.
 **/
void allocate_dir_v2(int fdHd, struct SuperBlock ReadBlock, char *dir_name, char *file_name){
    // * STEP 1 - Split the path into the directory name and the path itself.
    // * STEP 2 - Keep the reference to the father directory.
    // * STEP 3 - If father does not have first, then allocate the directory in the first.
    // *          Else, iterate into next's

    // * First it is necessary to split the path into the directory name and the path itself.

    char *dir_path = (char *)malloc(sizeof(char) * strlen(dir_name) + 1);
    strcpy(dir_path, dir_name);

    char *token = (char *)malloc(sizeof(char) * strlen(dir_name));
    token = strtok(dir_path, "/");

    char *nomeFormatado = (char *)malloc(sizeof(char) * strlen(dir_name));

    // * We have to iterate through the path, and make sure that this path exists and the folder does not.
    long int thisDirAddress = 0;
    long int beforeAddress = 0;

    // * Running into path splited by /
    // ? Returning the father path in the before address

    while (token != NULL)
    {
        // * See if is the last element
        // * If it is, then we have the directory name, and the father address.
        // * If the last string has a find_dir, then the directory already exists.
        
        beforeAddress = thisDirAddress;
        thisDirAddress = find_dir(fdHd, ReadBlock, thisDirAddress, token);
        strcpy(nomeFormatado, token);

        if (thisDirAddress == 0)
        {

            // check if directory already exists
            if (find_dir(fdHd, ReadBlock, thisDirAddress, token) != 0 )
            {
                printf("DiretÃ³rio jÃ¡ existe.\n");
                return;
            }

            if (strtok(NULL, "/") == NULL)
            {
                printf("Parou no token: %s\n", token);
                break;
            }

            printf("Caminho nÃ£o encontrado.\n");
        }

        strcpy(dir_path, token);
        token = strtok(NULL, "/");
        
    }
    printf("Endereco do before (pai): %ld\n", beforeAddress);
    printf("Endereco do elemento (this): %ld\n", thisDirAddress);

    // * If thisDirAddress is 0, then the directory does not exist
    // * If beforeAddress is 0, then the directory is the root directory

    if (thisDirAddress == 0){
        // * Creating the directory
        struct directory *directory_instance = (struct directory *)malloc(ReadBlock.block_size);
        strcpy(directory_instance->name, nomeFormatado);

        // * Allocating in the bitmap of data

        long int free_data_block = return_free_data_bit(fdHd, ReadBlock);

        if (free_data_block == -1)
        {
            printf("No free data blocks.\n");
            return;
        }


         // * Verify if the directory is the root directory
         // * If it is, then the father block will be in the super informations
         // * Otherwise, we need do the math to find the father block
        long int physicalFatherAddress = ReadBlock.inode_directory_start;
        if (beforeAddress != 0){
            physicalFatherAddress = beforeAddress * ReadBlock.block_size + 1;
        }

        // * Catch the father instance
        struct directory *father_directory = (struct directory *)malloc(ReadBlock.block_size);
        lseek(fdHd, physicalFatherAddress, SEEK_SET);
        read(fdHd, father_directory, ReadBlock.block_size);

        // * We have two cases:
        // * 1 - The father directory does not have a first directory
        // * 2 - The father directory has a first directory

        if (father_directory->first_int == 0){
            // * If he doesn't have a first directory, then we allocate the directory in the first
            father_directory->first_int = free_data_block;
            lseek(fdHd, physicalFatherAddress, SEEK_SET);
            write(fdHd, father_directory, ReadBlock.block_size);

        } else {
            // * Else we need to iterate through the next's of the father directory
            // * And allocate the directory in the last next
            struct directory *next_directory = (struct directory *)malloc(ReadBlock.block_size);
            long int physicalNextAddress = father_directory->first_int * ReadBlock.block_size + 1;
            
            // * Catching the next directory from memory
            lseek(fdHd, physicalNextAddress, SEEK_SET);
            read(fdHd, next_directory, ReadBlock.block_size);
            
            while (next_directory->next_int != 0){
                physicalNextAddress = next_directory->next_int * ReadBlock.block_size + 1;
                lseek(fdHd, physicalNextAddress, SEEK_SET);
                read(fdHd, next_directory, ReadBlock.block_size);
            }

            // * Allocating the directory in the last next
            // * And updating the next of the last next

            next_directory->next_int = free_data_block;
            lseek(fdHd, physicalNextAddress, SEEK_SET);
            write(fdHd, next_directory, ReadBlock.block_size);
        }
        
        // ? FIXED FILE TYPE, ALLOCATING STATIC TXT
        long int *indirects = (long int *)malloc(4 * sizeof(long int));
        if (strlen(file_name) > 1){
            indirects = allocate_data(fdHd, ReadBlock, file_name);

        }

        // * Allocating Inode

        long int inodeInt = allocate_inode(fdHd, ReadBlock, indirects);
        directory_instance->inode = inodeInt;

        // * Allocating the directory in the free data block
        long int physicalDirectoryAddress = physicalAddress(ReadBlock.block_size, free_data_block);
        lseek(fdHd, physicalDirectoryAddress, SEEK_SET);
        write(fdHd, directory_instance, ReadBlock.block_size);  

        printf("Diretorio criado com sucesso.\n");

    } else {
        printf("Diretorio jÃ¡ existe.\n");
    }


}


/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 24/04/2023.
 * \brief Function that finds a directory in memory, based on the folder name and the father's address
 * \param father_address address of the father directory, if is the root, it will be 0.
 * \param dir_name The directory's name.
 **/

// "/home/etc"

long int find_dir(int fdHd, struct SuperBlock ReadBlock, long int father_address, char *dir_name)
{
    // ! considerar first primeiro
    // * With the father address, we can find its first child.
    
    // * Directory struct instance.
    struct directory *directory_instance = returnDirPhysicalLocation(fdHd, ReadBlock, father_address);

    if (directory_instance->first_int == 0)
    {
        printf("Directory not found.\n");
        return 0;
    }

    // * Set the first child address
    long int dir_address = physicalAddress(ReadBlock.block_size, directory_instance->first_int);

    // * Begin the searching
    long int directory_address = 0;
    long int directory_block = directory_instance->first_int;
    while (directory_address == 0)
    {

        // * Read the directory
        lseek(fdHd, dir_address, SEEK_SET);
        read(fdHd, directory_instance, ReadBlock.block_size);

        // ! Showing the current directory for debug purposes
        // * Check if the directory is the one we are looking for
        
        if (strcmp(directory_instance->name, dir_name) == 0)
        {
            printf("Directory found in block %ld.\n", directory_block);
            printf("Directory name: %s\n", directory_instance->name);
            return directory_block;
        }
        if (directory_instance->next_int == 0)
        {
            printf("Directory not found.\n");
            return 0;
        }
        dir_address = directory_instance->next_int * ReadBlock.block_size + 1;
        directory_block = directory_instance->next_int;
    }
};

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 24/04/2023.
* \brief Function that shows all directories in memory, based on the folder name and the father's address

**/
void show_all_directories(int fdHd, struct SuperBlock ReadBlock)
{

    long int father_address = ReadBlock.inode_directory_start;
    struct directory *directory_instance = (struct directory *)malloc(ReadBlock.block_size);
    lseek(fdHd, father_address, SEEK_SET);
    read(fdHd, directory_instance, ReadBlock.block_size);
    printf("\nDirectory Father: %s \n", directory_instance->name);

    long int child_address = directory_instance->first_int;
    struct directory *child_instance = (struct directory *)malloc(ReadBlock.block_size);
    lseek(fdHd, child_address * ReadBlock.block_size + 1, SEEK_SET);
    read(fdHd, child_instance, ReadBlock.block_size);

    print_nexts(fdHd, ReadBlock, child_instance, child_address, 1);

};

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 01/05/2023.
* \brief Print all directories in memory, based on the folder name and the father's address

**/
void print_nexts(int fdHd, struct SuperBlock ReadBlock, struct directory *child_instance, long int child_address, int level){

   // * While child_instance exists do

        printf("Inode: %d", child_instance->inode);
        
        for (int i = 0; i < level; i++)
        {
            printf("--");
        }

        printf("Directory: %s \n", child_instance->name);
        // printf("Directory d: %ld \n", child_instance->next_int);
        // printf("Directory f: %ld \n", child_instance->first_int);
        if (child_instance->next_int == 0 && child_instance->first_int == 0)
        {
            return;
        }

        child_address = child_instance->next_int;
        long int child_physical_address = child_address * ReadBlock.block_size + 1;

        struct directory *first = (struct directory *)malloc(ReadBlock.block_size);
        lseek(fdHd, child_instance->first_int * ReadBlock.block_size + 1, SEEK_SET);
        read(fdHd, first, ReadBlock.block_size);
        
        if (child_instance->first_int != 0)
        {
            print_nexts(fdHd, ReadBlock, first, child_address, level + 1);
        }

        lseek(fdHd, child_physical_address, SEEK_SET);
        read(fdHd, child_instance, ReadBlock.block_size);
        
        if (child_physical_address > 1){
            print_nexts(fdHd, ReadBlock, child_instance, child_address, level);

        }

}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Function that allocates data in memory using the directs and indirects pointers of the inode.
 * \date 05/06/2023
 * 
 */

long int * allocate_data(int fdHd, struct SuperBlock ReadBlock, char *data){
    // ! First desconsidering the directs pointers
    // ! Indirect 1° is working, 2° is working, 3° in progress

    long int *indirectArray = malloc(4 * sizeof(long int));
    
    // * Information about the pointers
    // * each indirect pointer to a block of data (block_size or 4096 bytes)
    long int total_data_indirects_1 = ReadBlock.block_size / sizeof(long int);
    long int total_data_indirects_2 = pow(ReadBlock.block_size / sizeof(long int),2);
    long int total_data_indirects_3 = pow(ReadBlock.block_size / sizeof(long int),3);

    // * Alocando todos os blocos necessários de acordo com os indirects necessários

    // ? Considering a dump file data
    int text_file = open(data, O_RDONLY);

    char *buffer_data = (char *)malloc(ReadBlock.block_size);

    // * Parameter to see if there is a need to change the indirect pointer
    long int total_blocks_read = 0;

    // ? Indirect pointers counter
    long int * all_indirect_counter = malloc(3 * sizeof(long int));
    memset(all_indirect_counter, 0, 3 * sizeof(long int));
    // * First indirect long int pointer

    // ? Indirect pointers
    long int * all_active_indirect = malloc(3 * sizeof(long int));
    memset(all_active_indirect, 0, 3 * sizeof(long int));

    int reading_data = read(text_file, buffer_data, ReadBlock.block_size);

    while (reading_data > 0)
    {
        // * First Indirect
        long int block_address_allocate = return_free_data_bit(fdHd, ReadBlock);
        // * ifs control the indirect pointer
        if (total_blocks_read == 0){
            all_active_indirect[0] = block_address_allocate;
            indirectArray[0] = block_address_allocate;
            block_address_allocate = return_free_data_bit(fdHd, ReadBlock);
        } else if(total_blocks_read == total_data_indirects_1){
            all_active_indirect[0] = block_address_allocate;
            indirectArray[1] = block_address_allocate;
            block_address_allocate = return_free_data_bit(fdHd, ReadBlock);
            all_indirect_counter[0] = 0;
        } else if (total_blocks_read == total_data_indirects_2){
            all_active_indirect[0] = block_address_allocate;
            indirectArray[2] = block_address_allocate;
            block_address_allocate = return_free_data_bit(fdHd, ReadBlock);
            all_indirect_counter[0] = 0;
        } else if (total_blocks_read == total_data_indirects_3){
            printf("Error: File too big");
            break;
        } 

        if (total_blocks_read < total_data_indirects_1){
            write_indirect(fdHd, ReadBlock, buffer_data, block_address_allocate, all_indirect_counter[0], all_active_indirect[0],1);
            all_indirect_counter[0]++;
        } 
        else if (total_blocks_read < total_data_indirects_2){
            if (total_blocks_read % total_data_indirects_1 == 0){
                all_active_indirect[1] = block_address_allocate;
                write_indirect(fdHd, ReadBlock, buffer_data, block_address_allocate, all_indirect_counter[0], all_active_indirect[0],2);
                block_address_allocate = return_free_data_bit(fdHd, ReadBlock);
                all_indirect_counter[0]++;
                all_indirect_counter[1] = 0;
            }
            printf("new_block_address_allocate: %ld\n", block_address_allocate);
            write_indirect(fdHd, ReadBlock, buffer_data, block_address_allocate, all_indirect_counter[1], all_active_indirect[1],1);
            all_indirect_counter[1]++;
        } 
        else if (total_blocks_read < total_data_indirects_3){
            if (total_blocks_read % total_data_indirects_2 == 0){
                all_active_indirect[2] = block_address_allocate;
                write_indirect(fdHd, ReadBlock, buffer_data, block_address_allocate, all_indirect_counter[0], all_active_indirect[0],2);
                block_address_allocate = return_free_data_bit(fdHd, ReadBlock);
                all_indirect_counter[0]++;
                all_indirect_counter[1] = 0;
                all_indirect_counter[2] = 0;
            }
            if (total_blocks_read % total_data_indirects_1 == 0){
                all_active_indirect[1] = block_address_allocate;
                write_indirect(fdHd, ReadBlock, buffer_data, block_address_allocate, all_indirect_counter[1], all_active_indirect[1],2);
                block_address_allocate = return_free_data_bit(fdHd, ReadBlock);
                all_indirect_counter[1]++;
                all_indirect_counter[2] = 0;
            }
            write_indirect(fdHd, ReadBlock, buffer_data, block_address_allocate, all_indirect_counter[2], all_active_indirect[2],1);
            all_indirect_counter[2]++;
        
        }
        
        memset(buffer_data, 0, ReadBlock.block_size);

        reading_data = read(text_file, buffer_data, ReadBlock.block_size);

        total_blocks_read++;
        
    }

    // * Last of the array is the size of the file in blocks
    indirectArray[3] = total_blocks_read;
    return indirectArray;
}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Function that allocates data in first indirect inode.
 * \date 18/06/2023
 */

void write_indirect(int fdHd,
                    struct SuperBlock ReadBlock,
                    char *data, 
                    long int block_address_allocate, 
                    long int indirect_counter, 
                    long int active_indirect, 
                    int level){

    // * Write data Block

    if (level == 1){
        
        write_block(fdHd, ReadBlock, data, block_address_allocate);
    
    }

    long int physical_offset = physicalAddress(ReadBlock.block_size, active_indirect)+
                                (indirect_counter*sizeof(long int));
    off_t offset = lseek(fdHd,physical_offset, SEEK_SET);
    if (offset == -1) {
        printf("Error using lseek\n");
    }
    // * Write block in indirect

    long int *block_address_copy = (long int *)malloc(sizeof(long int));
    block_address_copy = &block_address_allocate;
    write(fdHd, block_address_copy, sizeof(long int));

    long int *block_address_read = (long int *)malloc(sizeof(long int));
    lseek(fdHd, physical_offset, SEEK_SET);
    read(fdHd, block_address_read, sizeof(long int));

}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Function that allocates data in first indirect inode.
 * \date 18/06/2023
 */

void write_block(int fdHd, struct SuperBlock ReadBlock, char *data, long int block_address){
        lseek(fdHd, physicalAddress(ReadBlock.block_size, block_address), SEEK_SET);
        write(fdHd, data, ReadBlock.block_size);
}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Function that reads the data.
 * \date 22/06/2023
*/

void read_data(int fdHd, struct SuperBlock ReadBlock, struct Inode *inode){
        
    // * Information about the pointers
    // * each indirect pointer to a block of data (block_size or 4096 bytes)
    // ! NEED REFACTORING
    long int total_data_indirects_1 = ReadBlock.block_size / sizeof(long int);
    long int total_data_indirects_2 = pow(ReadBlock.block_size / sizeof(long int),2);
    long int total_data_indirects_3 = pow(ReadBlock.block_size / sizeof(long int),3);

    long int total_size_block = inode->file_size;
    long int total_blocks_read = 0;

    long int active_indirect = 0;

    long int * all_indirects = malloc(sizeof(long int)*3);
    memset(all_indirects, 0, sizeof(long int)*3);

    while (total_blocks_read < total_size_block){
        if (all_indirects[0] == 0){
            all_indirects[0] = inode->indirect1;
            for (int i = 0; i < total_data_indirects_1; i++){
                read_block(fdHd, ReadBlock, all_indirects[0], i);

                total_blocks_read++;
                if (total_blocks_read == total_size_block){
                    printf("\nTotal blocks read 1: %ld \n", total_blocks_read);
                    break;
                }
            }
        } else if (total_blocks_read == total_data_indirects_1){
            all_indirects[0] = inode->indirect2;
            for (int i = 0; i < total_data_indirects_2; i++){
                long int physical_offset = physicalAddress(ReadBlock.block_size,all_indirects[0])+i*sizeof(long int);
                lseek(fdHd, physical_offset , SEEK_SET);
                read(fdHd, &all_indirects[1], sizeof(long int));
                for (int j = 0; j < total_data_indirects_1; j++){
                    read_block(fdHd, ReadBlock, all_indirects[1], j);
                    total_blocks_read++;
                    if (total_blocks_read == total_size_block){
                        printf("\nTotal blocks read 2: %ld \n", total_blocks_read);
                        break;
                    }
                }
                if (total_blocks_read == total_size_block){
                    printf("\nTotal blocks read 3: %ld \n", total_blocks_read);
                    break;
                }
            }
            break;
        } else if (total_size_block == total_data_indirects_2){
            all_indirects[2] = inode->indirect3;
        } else {
            printf("Error: File too big");
            break;
        }
    }

}

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Function that reads a block of data.
 * \date 23/06/2023
 * 
 */

void read_block(int fdHd, struct SuperBlock ReadBlock, long int block_address, long int i){
    long int numero = (long int)malloc(sizeof(long int));
    long int physical_offset_num = physicalAddress(ReadBlock.block_size,block_address)+i*sizeof(long int);
    lseek(fdHd, physical_offset_num , SEEK_SET);
    read(fdHd, &numero, sizeof(long int));

    char *buffer_data = (char *)malloc(ReadBlock.block_size);
    long int physical_offset = physicalAddress(ReadBlock.block_size, numero);
    lseek(fdHd, physical_offset, SEEK_SET);
    read(fdHd, buffer_data, ReadBlock.block_size);
    printf("%s", buffer_data);
}


/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \brief Create a dump directory tree object
 * \date 24/04/2023
 * \param fdHd 
 * \param ReadBlock 
 */
void create_dump_directory_tree(int fdHd, struct SuperBlock ReadBlock){

    allocate_dir_v2(fdHd, ReadBlock, "/pasta1","inputTest.txt");
    // allocate_dir_v2(fdHd, ReadBlock, "/pasta1/pasta1.1");
    // allocate_dir_v2(fdHd, ReadBlock, "/pasta1/pasta1.2");
    // allocate_dir_v2(fdHd, ReadBlock, "/pasta1/pasta1.1/pasta1.1.1");
    allocate_dir_v2(fdHd, ReadBlock, "/pasta2","inputTest2.txt");
    // allocate_dir_v2(fdHd, ReadBlock, "/pasta2/pasta2.1");
    // allocate_dir_v2(fdHd, ReadBlock, "/pasta3");
    // allocate_dir_v2(fdHd, ReadBlock, "/pasta4");
    // allocate_dir_v2(fdHd, ReadBlock, "/pasta5");
    show_all_directories(fdHd, ReadBlock);
    read_inode(fdHd, ReadBlock, 2);
    read_inode(fdHd, ReadBlock, 3);

}