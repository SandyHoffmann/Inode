#include "bitmap.h"
#include "super.h"

int return_free_inode_bit(int fdHd, struct SuperBlock ReadBlock){
    int *bufferBitmapInode = malloc(ReadBlock.inode_total/8);
    
    lseek(fdHd, ReadBlock.bmap_inode_start, SEEK_SET);
    read(fdHd, bufferBitmapInode, ReadBlock.inode_total/8);

    for (int i = 0; i < ROUND(ReadBlock.inode_total/8); i++)
    {   
        // * Byte = ff or 255 = full

        if (bufferBitmapInode[i] < 255)
        {
            // * Free bit position
            // 11110000 -> 11111000
            printf("\nByte: %d", bufferBitmapInode[i]);
            unsigned int free_bit = i;

            // * Write 1 in the free bit
            unsigned int mask = 0b10000000;

            free_bit |= mask;

            lseek(fdHd, ReadBlock.bmap_inode_start + i, SEEK_SET);
            write(fdHd, &free_bit, 1);
            
            // * reaching the address of the free bit
            // int address = ReadBlock.inode_start + (free_bit * ReadBlock.inode_size);

            return i;
        }
    }
    printf("\nNao ha bits de inode livres\n");
    return 2;

}

int return_free_data_bit(int fdHd, struct SuperBlock ReadBlock){
    char *bufferBitmapData = malloc(ReadBlock.block_data_total/8);

    lseek(fdHd, ReadBlock.bmap_data_start, SEEK_SET);
    read(fdHd, bufferBitmapData, ReadBlock.block_data_total);

    for (int i = 0; i < ReadBlock.block_data_total; i++)
    {
        if (bufferBitmapData[i] == 0)
        {
            // * Free bit position
           
            int free_bit = i;
           
            // * Write 1 in the free bit
           
            bufferBitmapData[i] = 1;
           
            // * Write the buffer in the HD, after writing the buffer
            // ! This is rewriting the whole bitmap inode, not just the free bit !

            lseek(fdHd, ReadBlock.bmap_data_start, SEEK_SET);
            write(fdHd, bufferBitmapData, ReadBlock.block_data_total);
            
            // * reaching the address of the free bit
            int address = ReadBlock.data_start + (free_bit * ReadBlock.block_size);

            return address;
        }
    }

    printf("\nNao ha bits de data livres\n");
    return -1;

};