#include "inode.h"
#include "file_operations.h"
/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 23/04/2023.
 * \brief Function that creates the inode itself.
 * \param content The content of the file itself.
 * \param file_type The type of the file content (directory, regular file, ...).
 **/
struct Inode *create_inode(char *content, int file_type, long int *indirects)
{
    struct Inode *inode = (struct Inode *)malloc(sizeof(struct Inode));
    inode->file_type = file_type;
    inode->creation_time = time(NULL);
    inode->modify_time = time(NULL);
    if (indirects != NULL)
    {
        printf("Indirects 1 create inode: %ld\n", indirects[0]);
        inode->indirect1 = indirects[0];
        inode->indirect2 = indirects[1];
        inode->indirect3 = indirects[2];
        inode->file_size = indirects[3];
    }
    else
    {
        inode->indirect1 = 0;
        inode->indirect2 = 0;
        inode->indirect3 = 0;
        inode->file_size = 0;
    }

    printf("Inode size: %ld\n", sizeof(struct Inode));

    if (strlen(content) == 0)
    {
        return inode;
    }
    // ! ... Indirect logic here ...
    return inode;
}

long int allocate_inode(int fdHd, struct SuperBlock ReadBlock, long int * indirects)
{
    printf("Allocate inode\n");
    printf("Indirect 1: %ld\n", indirects[0]);

    long int free_inode = return_free_inode_bit(fdHd, ReadBlock);
    struct Inode *inode_instance = (struct Inode *)malloc(ReadBlock.inode_size);
    inode_instance = create_inode("", 2, indirects);
    long int physical_inode_address = ReadBlock.inode_start + free_inode * ReadBlock.inode_size;
    printf("Physical inode address: %ld\n", physical_inode_address);
    lseek(fdHd, physical_inode_address, SEEK_SET);
    write(fdHd, inode_instance, ReadBlock.inode_size);

    return free_inode;
}

/**
 * @authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * @brief Read an inode
 * @date 22/06/2023
 * 
 * @param fdHd 
 * @param ReadBlock 
 * @param inode_address - The address of the inode to be read in block
 */
void read_inode(int fdHd, struct SuperBlock ReadBlock, long int inode_address){
    long int physical_inode_address = ReadBlock.inode_start + inode_address * ReadBlock.inode_size;
    lseek(fdHd, physical_inode_address, SEEK_SET);
    struct Inode *inode = (struct Inode *)malloc(ReadBlock.inode_size);
    read(fdHd, inode, ReadBlock.inode_size);
    printf("inode->file_type: %d\n", inode->file_type);
    printf("inode->file_size: %d\n", inode->file_size);
    printf("inode->creation_time: %ld\n", inode->creation_time);
    printf("inode->modify_time: %ld\n", inode->modify_time);
    printf("inode->indirect1: %ld\n", inode->indirect1);
    printf("inode->indirect2: %ld\n", inode->indirect2);
    printf("inode->indirect3: %ld\n", inode->indirect3);
}

/**
 * @authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * @brief Read an inode data
 * @date 22/06/2023
 * 
 * @param fdHd 
 * @param ReadBlock 
 * @param inode_address - The address of the inode to be read in block
 */

void read_inode_data(int fdHd, struct SuperBlock ReadBlock, long int inode_address){
    long int physical_inode_address = ReadBlock.inode_start + inode_address * ReadBlock.inode_size;
    lseek(fdHd, physical_inode_address, SEEK_SET);
    struct Inode *inode = (struct Inode *)malloc(ReadBlock.inode_size);
    read(fdHd, inode, ReadBlock.inode_size);
    read_data(fdHd, ReadBlock, inode);
}
