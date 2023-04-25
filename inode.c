#include "inode.h"

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 23/04/2023.
* \brief Function that creates the inode itself.
* \param content The content of the file itself.
* \param file_type The type of the file content (directory, regular file, ...).
**/
struct Inode *create_inode(char *content, int file_type){
    struct Inode *inode = (struct Inode *) malloc(sizeof(struct Inode));
    inode->file_type = file_type;
    inode->file_size = strlen(content);
    inode->creation_time = time(NULL);
    inode->modify_time = time(NULL);
   

    if (strlen(content) == 0){
        return inode;
    }
    // ! ... Indirect logic here ...
    return inode;
}