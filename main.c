#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "super.c"
#include "bitmap.c"
#include "directory.c"
#include "inode.c"
#include "file_operations.c"
#include <unistd.h>

/**

Criar HD = dd if=/dev/zero of=bla.hd bs=GB count=2

Hexdump hexdump -n512 -s60 -c bla.hd
-n = number of bytes to dump (bytes)
-s = offset to start dumping from (bytes)

xxd -s 4594 -l 512 -b bla.hd
-s = start
 -l = quantity

*/

int fdHd;

void open_hd();
void close_hd();

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
    open_hd();
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

void debug(int fdHd)
{
    // * Read SuperBlock
    struct SuperBlock ReadBlock;
    char *buffer = malloc(4096);
    lseek(fdHd, 0, SEEK_SET);
    read(fdHd, buffer, 4096);
    memcpy(&ReadBlock, buffer, sizeof(struct SuperBlock));
    printf("\nBlock size lido: %d", ReadBlock.block_size);
    printf("\nInode size lido: %d", ReadBlock.inode_size);
    printf("\nInode per block lido: %d", ReadBlock.inode_per_block);
    printf("\nInode start lido: %d", ReadBlock.inode_start);
    printf("\nInode directory start lido: %d", ReadBlock.inode_directory_start);
    printf("\nData start lido: %d", ReadBlock.data_start);
    printf("\nMagic Number: %ld", ReadBlock.magic);
    printf("\nInode Bitmap Start: %d", ReadBlock.bmap_inode_start);
    printf("\nData Bitmap Start: %d", ReadBlock.bmap_data_start);
    printf("\nInode Total: %d", ReadBlock.inode_total);
    printf("\nBlock Data Total: %d", ReadBlock.block_data_total);

    // * Read Inode Bitmap
    // * Starts in 4097 byte
    // * Finishes in 36361 byte
    // * Total = 36361 (fim) - 4097 (inicio) = 32264 * 8 (bits in 1 byte) = 258112 bits

    char *bufferBitmapInode = malloc(ReadBlock.inode_total / 8);
    lseek(fdHd, ReadBlock.bmap_inode_start, SEEK_SET);
    read(fdHd, bufferBitmapInode, ReadBlock.inode_total / 8);
    printf("\nInode Bitmap: %s", bufferBitmapInode);

    // * Read Data Bitmap
    // * Starts in 4594 byte
    // * Finishes in 8692 byte
    // * Total = 8692 (fim) - 4594 (inicio) = 4098 * 8 (bits in 1 byte) = 32784 bits

    char *bufferBitmapData = malloc(ReadBlock.block_data_total / 8);
    lseek(fdHd, ReadBlock.bmap_data_start, SEEK_SET);
    read(fdHd, bufferBitmapData, ReadBlock.block_data_total / 8);
    printf("\nData Bitmap: %s", bufferBitmapData);

    // * Read Inode
    struct Inode *firstInode = (struct Inode *)malloc(sizeof(struct Inode));
    lseek(fdHd, ReadBlock.inode_start, SEEK_SET);
    read(fdHd, firstInode, ReadBlock.inode_size);

    printf("\nFile type: %d", firstInode->file_type);
    printf("\nFile size: %d", firstInode->file_size);

    // * Read first directory

    struct directory *firstDirectory = (struct directory *)malloc(ReadBlock.block_size);
    lseek(fdHd, ReadBlock.inode_directory_start, SEEK_SET);
    read(fdHd, firstDirectory, ReadBlock.block_size);

    printf("\nFirst Directory: %s", firstDirectory->name);
}

void resetHdF()
{
    char buffer[4096];
    bzero(buffer, 4096);
    lseek(fdHd, 0, SEEK_SET);
    int totalHd1gb = (1024 * 1024 * 1024) / 4096;
    for (int i = 0; i < totalHd1gb; i++)
    {
        write(fdHd, buffer, 4096);
    }
}

void directory_test(int fdHd)
{
    resetHdF();
    sleep(2);
    disk_format(fdHd);
    sleep(2);

    struct SuperBlock ReadBlock;
    char *buffer = malloc(4096);
    lseek(fdHd, 0, SEEK_SET);
    read(fdHd, buffer, 4096);
    memcpy(&ReadBlock, buffer, sizeof(struct SuperBlock));

    create_dump_directory_tree(fdHd, ReadBlock);
}

void main()
{

    open_hd();

    directory_test(fdHd);

    close_hd();
}

void open_hd()
{
    printf("Abrindo HD...");
    if ((fdHd = open("bla.hd", O_RDWR)) == -1)
    {
        printf("ERRO");
        exit(1);
    }
}

void close_hd()
{
    close(fdHd);
}


