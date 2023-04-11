#include "super.c"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int fdHd;

void open_hd();
void close_hd();
char *read_block(int block_num);

void main(){
    struct SuperBlock block = create_super_block(4096, 64, 1);
    void *buffer = malloc(block.block_size);
    // memcpy(buffer, &block, sizeof(struct SuperBlock));
    open_hd();

    struct SuperBlock ReadBlock;

    // lseek(fdHd, 0, SEEK_SET);
    // write(fdHd, buffer, block.block_size);
    // free(buffer);

    lseek(fdHd, 0, SEEK_SET);
    read(fdHd, buffer, block.block_size);

    memcpy(&ReadBlock, buffer, sizeof(struct SuperBlock));

    printf("\nBlock size lido: %d", ReadBlock.block_size);
    printf("\nInode size lido: %d", ReadBlock.inode_size);
    printf("\nInode per block lido: %d", ReadBlock.inode_per_block);

    free(buffer);

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

char *read_block(int block_num)
{
}


