#include "bitmap.h"
#include "super.h"
#define SETBIT(a, n) ((a << n)+ 1)           // a entry value and n is the bit to set
#define CLEARBIT(a, n) (a & (0xff ^ (1 << n))) // a entry value and n is the bit to set

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 01/05/2023.
* \updated 20/06/2023.
* \brief Bitmap functions.

**/



int return_free_inode_bit(int fdHd, struct SuperBlock ReadBlock)
{
    return return_free_bit(fdHd, ReadBlock.inode_total, ReadBlock.bmap_inode_start);
}

int return_free_data_bit(int fdHd, struct SuperBlock ReadBlock)
{
    return return_free_bit(fdHd, ReadBlock.block_data_total, ReadBlock.bmap_data_start);
};

unsigned char return_bit_moved(unsigned char bitset, int i){
    printf("\nBitset: %d\n", bitset);
    unsigned char bit = (bitset >> 7 - i) & 1;
    return bit || 0b10000000;
}

int return_free_bit(int fdHd, long int total, long int bmap_start)
{
    char *bufferBitmap = (char *)malloc(total / 8);

    lseek(fdHd, bmap_start, SEEK_SET);
    read(fdHd, bufferBitmap, total / 8);

    for (int i = 0; i < total / 8; i++)
    {
        char hex_string[3];
        sprintf(hex_string, "%02hhx", bufferBitmap[i]);
        int statusBitMap = strtol(hex_string, NULL, 16);

        // ! Arrumando bitmap (dnv)

        // * Byte = ff or 255 = full
        // ? Ideia = << 1 + 1
        if (statusBitMap < 255)
        {
            int allocated = SETBIT(statusBitMap,1);
            // printf("location: %ld\n", bmap_start+i);
            long int casas = __builtin_popcount(allocated);
            // printf("\nAllocated: %d\n", allocated);
            lseek(fdHd, bmap_start + i, SEEK_SET);
            write(fdHd, &allocated, 1);

            // * reaching the address of the free bit

            return (i * 8) + casas;
        }
    }
    printf("\nNão há bits livres\n");
    return -1;
};