/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 25/03/2023.
 **/

#include "directory.h"

/**
 * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
 * \since 25/03/2023.
 * \brief Function that creates a directory, that is represented by a leaf in the multiway tree. The address of it in the memory is returned.
 * \param _name The directory's name.
 * \param _inode The inode's number.
 **/
Directory *create_dir(char *_name, int _inode)
{
    Directory *dir = (Directory *)malloc(sizeof(Directory));
    strcpy(dir->name, _name);
    dir->inode = _inode;
    dir->first_int = 0;
    dir->next_int = 0;
    return dir;
}