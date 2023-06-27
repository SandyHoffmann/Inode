#include "user_interaction.h"
#include "super.h"
#include "bitmap.h"
#include "directory.h"
#include "inode.h"
#include "file_operations.h"

void user_menu(int fdHd){
    printf("Welcome to the Inode File System!\n");
    printf("We will format your disk now.\n");
    sleep(1);
    disk_format(fdHd);
    sleep(2);
    printf("Disk formatted!\n");

    printf("Now, choose whats next.\n");

    int option = 0;
    struct SuperBlock ReadBlock = read_superblock(fdHd);

    while (option != 5){
        printf("\n*--------------------------------*\n");
        printf("1. Show the superblock info.\n");
        printf("2. Create a file.\n");
        printf("3. List all files and directories.\n");
        printf("4. Show a file content.\n");
        printf("5. Exit.\n");
        scanf("%d", &option);

        switch (option){
            case 1:
                printf("Showing info.\n");
                show_superblock(fdHd);
                break;
            case 2:
                printf("Creating a dumb directory three.\n");
                create_dump_directory_tree(fdHd, ReadBlock);
                break;
            case 3:
                printf("Listing all files and directories.\n");
                show_all_directories(fdHd, ReadBlock);
                break;
            case 4:
                printf("Give me the inode number: \n");
                long int inode_number = 0;
                scanf("%ld", &inode_number);
                read_inode(fdHd, ReadBlock, inode_number);
                printf("Show data? (y/n)\n");
                char show_data = 'n';
                scanf(" %c", &show_data);
                if (show_data == 'y'){
                    printf("Showing a file content.\n");
                    read_inode_data(fdHd, ReadBlock, inode_number);
                }
                break;
            case 5:
                printf("Exiting.\n");
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
        printf("\n*--------------------------------*\n");

    }
}


/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 23/04/2023.
 * \brief Formating a disk in Inode system means:
 * 1. Creating a superblock.
 * 1.1 Superblock will contain all the information about the disk.
 * 1.2 Superblock will be the first block of the disk.
 * 2. Allocate the first directory entry (root directory) = /.
 * 2.1 Allocate the inode for the root directory.
 * 3. Allocating the inode bitmap including this new instance of root directory.
 * 4. Allocating the data bitmap including the superblock location, inode bitmap, data bitmap, the root directory, and inode reserved sector.
 * \param fdhd The file containing the hd itself.
 **/
void disk_format(int fdHd)
{
    // * Inode size increased to 128 bytes (22/06/2023)
    struct SuperBlock block = create_super_block(4096, 128, 1);
    void *buffer = malloc(block.block_size);
    memcpy(buffer, &block, sizeof(struct SuperBlock));
    lseek(fdHd, 0, SEEK_SET);
    write(fdHd, buffer, block.block_size);
    free(buffer);

    // * Inode bitmap already mounted on hd
    int bit_free_inode = return_free_inode_bit(fdHd, block);

    Directory *root = create_dir("/", bit_free_inode);
    struct Inode *inode = create_inode("", 1, NULL);

    // * Write Inode in HD
    long int adress_first_inode = block.inode_start;
    lseek(fdHd, adress_first_inode, SEEK_SET);
    write(fdHd, inode, block.inode_size);

    // * Write Directory in HD
    long int adress_dir = block.inode_directory_start;
    lseek(fdHd, adress_dir, SEEK_SET);
    write(fdHd, root, sizeof(Directory));

    // * Data bitmap represents the data blocks that are free or not.

    long int total_data = block.data_start;                     // * bytes 0 - 16527862 byte
    int occupied_blocks = ROUND(total_data / block.block_size); // * blocks = 4036 occupied blocks


    for (int i = 0; i < occupied_blocks; i++)
    {
        return_free_data_bit(fdHd, block);
    }

    printf("Occupied blocks: %d\n", occupied_blocks);

    // for (int i = 0; i < total_blocks_full_byte + 1; i++){
    //     printf("Byte 2: %x\n", list_data_byte[i]);
    // }

    // list_data_bits[total_data_bits - 1] = resto;

    // * 11111111110000000000000000000000000000000000
    // * 1 byte = 8 bits
    // * 1 byte completo = 11111111
    // write(fdHd, list_data_bits, total_data_bits);
}
