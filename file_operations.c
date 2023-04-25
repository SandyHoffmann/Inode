
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
* \since 24/04/2023.
* \brief Function that allocates a directory in memory, creating the structure based on the absolute path.
* \param _name The directory's path, starting at /, last word will be taken as folder path.
**/
void allocate_dir(int fdHd, struct SuperBlock ReadBlock, char *dir_name){
    // * First it is necessary to split the path into the directory name and the path itself.

    char *dir_path = (char *) malloc(sizeof(char) * strlen(dir_name)+1);
    strcpy(dir_path, dir_name);

    char *token = (char *) malloc(sizeof(char) * strlen(dir_name));
    token = strtok(dir_path, "/");
    printf("token dividido\n");

    // * We have to iterate through the path, and make sure that this path exists and the folder does not.
    long int fatherAddress = 0;
    while (token != NULL){
        // * See if is the last element
        // * If it is, then we have the directory name, and the father address.
        // * If the last string has a find_dir, then the directory already exists.

        fatherAddress = find_dir(fdHd, ReadBlock, fatherAddress, token);
        printf("fatherAddress: %ld\n", fatherAddress);
        if (fatherAddress == 0){
            
            if (strtok(NULL, "/") == NULL){
                dir_name = token;
                break;
            }

            printf("Path not found.\n");
            
        }
        
        strcpy(dir_path, token);
        token = strtok(NULL, "/");
        
    }
    // strcpy(dir_name, token);
    if (fatherAddress != 0){
        // * Directory path exists and folder does not.
        // * Create the directory.

        // * Directory struct instance.
        struct directory * directory_instance = (struct directory * )malloc(ReadBlock.block_size);
        strcpy(directory_instance->name, dir_name);

        struct directory * father_directory = (struct directory * )malloc(ReadBlock.block_size);
        lseek(fdHd, fatherAddress, SEEK_SET);
        read(fdHd, father_directory, ReadBlock.block_size);

        printf("father_directory->name: %s\n", father_directory->name);

        
        // // * Now we have the last child of the father directory.
        // // * We need to allocate the new directory in the end of the list.

        // * Calculate the new address of the directory
        // ! For now let's allocate the directory in an data block. (I think we can divide the directories to share the same data block if they are small enough)
        long int free_data_block = return_free_data_bit(fdHd, ReadBlock);
        printf("free_data_block: %ld\n", free_data_block);
        if (free_data_block == -1){
            printf("No free data blocks.\n");
            return;
        }

        // * Finding last child of the father directory
        // ! considerar first primeiro
        printf("father_directory\n");
        // printf("father_directory->first->first_int: %ld\n", father_directory->first->first_int);
        if (father_directory->first_int == 0){
            printf("father_directory->first == NULL\n");
            father_directory->first_int = free_data_block;
            printf("first_int: %ld\n", father_directory->first_int);

            // * Write the father directory, 
            // * Se for diretorio fixo, fatherAddress basta. Se for dinamico fazer a conta: fatherAddress * ReadBlock.block_size + 1

            lseek(fdHd, fatherAddress, SEEK_SET);
            write(fdHd, father_directory, ReadBlock.block_size);
        } else {
            // ! considerar first
            printf("father_directory->first %ld\n", father_directory->first_int);
            printf("bb\n");
            long int childAddress = father_directory->first_int * ReadBlock.block_size + 1;
            lseek(fdHd, childAddress, SEEK_SET);
            read(fdHd, father_directory, ReadBlock.block_size);
            printf("aaaa\n");
            while (father_directory->next_int != 0){
                lseek(fdHd, childAddress, SEEK_SET);
                read(fdHd, father_directory, ReadBlock.block_size);
                childAddress = father_directory->next_int * ReadBlock.block_size + 1;
            }
            
            father_directory->next_int = free_data_block;
            // * Write the father directory, 
            // * Se for diretorio fixo, fatherAddress basta. Se for dinamico fazer a conta: fatherAddress * ReadBlock.block_size + 1
            printf("childAddress: %ld\n", childAddress);

            lseek(fdHd, childAddress, SEEK_SET);
            write(fdHd, father_directory, ReadBlock.block_size);

        }

        long int new_dir_address = free_data_block * ReadBlock.block_size + 1;
        printf("new_dir_address: %ld\n", new_dir_address);
        // * Write the new directory
        printf("new_dir_address: %ld\n", new_dir_address);
        lseek(fdHd, new_dir_address, SEEK_SET);
        write(fdHd, directory_instance, ReadBlock.block_size);
        
        printf("Directory created.\n");
    }
};


/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 24/04/2023.
* \brief Function that finds a directory in memory, based on the folder name and the father's address
* \param father_address address of the father directory, if is the root, it will be 0.
* \param dir_name The directory's name.
**/

// "/home/etc"
           
long int find_dir(int fdHd, struct SuperBlock ReadBlock,long int father_address, char *dir_name){
    // ! considerar first primeiro
    long int dir_address = ReadBlock.inode_directory_start;
    
    if (father_address == 0){
        return dir_address;
    }

    // * Directory struct instance.
    struct directory * directory_instance = (struct directory * )malloc(ReadBlock.block_size);
    
    // * Begin the searching
    long int directory_address = 0;
    while (directory_address == 0){

        // * Read the directory
        lseek(fdHd, dir_address, SEEK_SET);
        read(fdHd, directory_instance, ReadBlock.block_size);
        
        // ! Showing the current directory for debug purposes
        printf("Current directory: %s \n", directory_instance->name);

        // * Check if the directory is the one we are looking for
        if (strcmp(directory_instance->name, dir_name) == 0){
            return dir_address;
            
        }
        if (directory_instance->next_int == 0){
            printf("Directory not found.\n");
            return 0;
        }
        dir_address = directory_instance->next_int;
    }



};


/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 24/04/2023.
* \brief Function that shows all directories in memory, based on the folder name and the father's address

**/
void show_all_directories(int fdHd, struct SuperBlock ReadBlock){

    long int father_address = ReadBlock.inode_directory_start;
    struct directory * directory_instance = (struct directory * )malloc(ReadBlock.block_size);
    lseek(fdHd, father_address, SEEK_SET);
    read(fdHd, directory_instance, ReadBlock.block_size);
    printf("Directory Father: %s \n", directory_instance->name);
    printf("Directory Child d: %ld \n", directory_instance->first_int);

    long int child_address = directory_instance->first_int;
    struct directory * child_instance = (struct directory * )malloc(ReadBlock.block_size);
    lseek(fdHd, child_address * ReadBlock.block_size + 1, SEEK_SET);
    read(fdHd, child_instance, ReadBlock.block_size);
    printf("Directory Child: %s \n", child_instance->name);
    printf("Directory Child d: %ld \n", child_instance->next_int);
    if (child_address && child_instance->next_int != 0){
        child_address = child_instance->next_int;
        child_address = child_address * ReadBlock.block_size + 1;
    
        lseek(fdHd, child_address, SEEK_SET);
        read(fdHd, child_instance, ReadBlock.block_size);

        printf("Directory: %s \n", child_instance->name);

    }

    // * Directory struct instance.
    // while (father_address != 0){
    //     lseek(fdHd, father_address, SEEK_SET);
    //     read(fdHd, directory_instance, ReadBlock.block_size);
    //     printf("Directory: %s \n", directory_instance->name);
    //     while (directory_instance->next != NULL || directory_instance->next != 0){
    //         lseek(fdHd, father_address, SEEK_SET);
    //         read(fdHd, directory_instance, ReadBlock.block_size);
    //         printf("Directory: %s \n", directory_instance->name);
    //     }
    //     break;
    // }

    
    
};
