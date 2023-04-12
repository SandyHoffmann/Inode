/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
**/

#include "super.h"

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that creates the superblock for the file system based on the given parameters. The create superblock is returned.
* \param size_block The size of each block in bytes.
* \param size_inode The size of each inode in bytes.
* \param size_hd_gb The size of the hard drive in gigabytes.
**/
struct SuperBlock create_super_block(int size_block, int size_inode, int size_hd_gb){
    // * Calculate the necessary data to create the superblock.
    struct SuperBlock super_block = data_calculation_hd(size_block, size_inode, size_hd_gb);

    // * Show the created superblock.
    printf("Superblock created: \n");
    printf("Magic: %ld \n", super_block.magic);
    printf("Block size: %d \n", super_block.block_size);
    printf("Inode size: %d \n", super_block.inode_size);
    printf("Inode per block: %d \n", super_block.inode_per_block);
    printf("Reserved blocks: %d \n", super_block.reserved_blocks);
    printf("Bmap inode start: %d \n", super_block.bmap_inode_start);
    printf("Bmap data start: %d \n", super_block.bmap_data_start);
    printf("Inode start: %d \n", super_block.inode_start);
    printf("Inode directory start: %d \n", super_block.inode_directory_start);
    printf("Inode total: %d \n", super_block.inode_total);
    printf("Block data total: %d \n", super_block.block_data_total);
    printf("Data start: %d \n", super_block.data_start);

    return super_block;
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that calculates and returns the data necessary to create the superblock, since we'll be using them a lot.
* \param size_block The size of each block in bytes.
* \param size_inode The size of each inode in bytes.
* \param size_hd_gb The size of the hard drive in gigabytes.
**/
struct SuperBlock data_calculation_hd(int size_block, int size_inode, int size_hd_gb){
    // * Calculate the number of reserved blocks for the superblock.
    int reserved_block_superblock = 1;

    // * Calculate the number of data blocks in each inode.
    int number_data_in_inode = ROUND(size_block/ sizeof(long int));

    // * Calculate the size of each inode.
    long int direct_inode_only = 6 * size_block;
    long int direct_inode_plus_1ind = direct_inode_only + number_data_in_inode * size_block ;
    long int direct_inode_plus_2ind = direct_inode_plus_1ind + pow(number_data_in_inode, 2) * size_block;
    long int direct_inode_plus_3ind = direct_inode_plus_2ind + pow(number_data_in_inode, 3) * size_block;

    // * Print the inode data calculations.
    printf("Number of data in inode: %d bytes\n", number_data_in_inode);
    printf("Direct inode only: %ld bytes\n", direct_inode_only);
    printf("Direct inode plus 1 indirect: %ld bytes\n", direct_inode_plus_1ind);
    printf("Direct inode plus 2 indirect: %ld bytes\n", direct_inode_plus_2ind);
    printf("Direct inode plus 3 indirect: %ld bytes\n",
    printf("Direct inode plus 3 indirect: %ld bytes\n", direct_inode_plus_3ind);

    printf("Direct inode plus 1 indirect: %ld gb\n", direct_inode_plus_1ind/(1024*1024*1024));
    printf("Direct inode plus 2 indirect: %ld gb\n", direct_inode_plus_2ind/(1024*1024*1024));
    printf("Direct inode plus 3 indirect: %ld gb\n", direct_inode_plus_3ind/(1024*1024*1024));


    long long size_hb_bytes =  (long long) size_hd_gb * 1024 * 1024 * 1024;

    printf("Size of hard disk: %lld bytes\n", size_hb_bytes);

    // * Calculate the number of inodes, it will be stored in the superblock

    long int number_inodes = size_hb_bytes / (size_inode + size_block);

    printf("Number of reserved inodes: %ld\n", number_inodes);
    printf("Number of reserved inodes mb: %ld\n", number_inodes * size_inode / (1024*1024));

    // * Calculate the reserved block total number that will be reserved for inode sector

    long int number_inode_per_block = ROUND(size_block/size_inode);
    long int number_block_need_for_inode = ROUND(number_inodes/number_inode_per_block);

    printf("Number of inode per block: %ld inodes \n", number_inode_per_block);
    printf("Number of block need for inode: %ld blocks \n", number_block_need_for_inode);

    long int sector_size_reserved_for_inode = number_block_need_for_inode * size_block;

    printf("Sector size reserved for inode: %ld bytes \n", sector_size_reserved_for_inode);
    printf("Sector size reserved for inode: %ld mb \n", sector_size_reserved_for_inode/(1024*1024));
    
    // * bitmap for inode = number_block_need_for_inode/number_inode_per_block

    long int number_block_for_inode = ROUND(number_block_need_for_inode/number_inode_per_block);

    printf("Number of bitmaps for inode (bitmap): %ld bits \n", number_block_for_inode);

    // * bitmap for data = (size_hb_bytes - sector_size_reserved_for_inode) / size_block

    long int number_block_for_data = (size_hb_bytes - sector_size_reserved_for_inode) / size_block;

    printf("Number of bitmaps for data (bitmap): %ld bits \n", number_block_for_data);

    // * Considering the organization:
    /*
    * 1 - Superblock
    * 2 - Bitmap for inode
    * 3 - Bitmap for data
    * 4 - Inode
    * 5 - First directory entry (root)
    * 6 - Data
    */
    
    // * Calculate the reserved block total number that will be reserved for data sector
    int reserved_sector = reserved_block_superblock * size_block;
    int block_data_start = reserved_sector + number_block_need_for_inode + 1;
    int inode_start = block_data_start + number_block_for_data + 1;
    long int inode_directory_start = inode_start + (number_inodes * size_inode) + 1;
    long int data_start = inode_directory_start + size_inode + 1;
    struct SuperBlock super_block = {
        .magic = 0xf0f03410,
        .block_size = size_block,
        .inode_size = size_inode,
        .inode_per_block = number_inode_per_block,
        .reserved_blocks = 1,
        .bmap_inode_start = reserved_sector + 1,
        .bmap_data_start = block_data_start,
        .inode_start = inode_start,
        .inode_directory_start = inode_directory_start,
        .inode_total = number_inodes,
        .block_data_total = number_block_for_data,
        .data_start = data_start
    };

    return super_block;
}