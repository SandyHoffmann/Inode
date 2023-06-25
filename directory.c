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

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that inserts a directory into another directory.
// * \param dir The directory.
// * \param sub_dir The directory that'll be subdirectory of dir.
// **/
// void dir_insert(Directory* dir, Directory* sub_dir){
//     sub_dir->next = dir->first;
//     dir->first = sub_dir;
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that executes a command similar to 'ls'.
// * \param dir The directory where the command will be executed.
// **/
// void ls_dir(Directory* dir){
//     Directory *d = dir->first;
//     while (d != NULL){
//         printf("\n%s", d->name);
//         d = d->next;
//     }
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that prints (in Preorder) all the directory hierarchy of the given directory and its subdirectories.
// * \param dir The root directory where the print starts.
// **/
// void print_tree(Directory* dir){
//     printf("\n%s", dir->name);
//     Directory *d = dir->first;

//     while (d != NULL){
//         // Recursively call print_tree() for all subdirectories.
//         print_tree(d);
//         d = d->next;
//     }
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that returns 1 if a directory pertains to a given directory or 0 if not.
// * \param dir The root directory where the search will be executed.
// * \param _name The directory's name that'll be searched.
// **/
// int dir_pertain(Directory* dir, char* _name){
//     if (dir == NULL) return 0;
//     int equal = strcmp(dir->name, _name);
//     return (!equal) || (dir_pertain(dir->first, _name)) || (dir_pertain(dir->next, _name));
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that releases the allocated memory of the tree's directory.
// * \param dir The root directory itself.
// **/
// void release_dir(Directory* dir){
//     if (dir != NULL){
//         release_dir(dir->first);
//         release_dir(dir->next);
//         free(dir);
//     }
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that returns the number of leafs, i.e, the number of files or directories with no subdirectories or files in it.
// * \param dir The root directory itself.
// **/
// int number_of_leafs(Directory* dir){
//     if (dir == NULL) return 0;
//     int leafs = 0;
//     Directory* children_dir = dir->first;
//     while (children_dir != NULL){
//         Directory* sis_dir = children_dir->next;
//         while (sis_dir != NULL){
//             leafs += number_of_leafs(sis_dir);
//             sis_dir = sis_dir->next;
//         }
//         children_dir = children_dir->first;
//     }
//     return ++leafs;
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that returns the tree's height of the directories hierarchy.
// * \param dir The root directory itself.
// **/
// int dir_height(Directory* dir){
//     int hMax = -1;
//     Directory *d = dir->first;
//     while (d != NULL){
//         int h = dir_height(d);
//         if (h > hMax) hMax = h;
//         d = d->next;
//     }
//     return ++hMax;
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that returns 1 if directory dir1 is equal to directory dir2, or 0 if not.
// * \param dir1 The directory dir1.
// * \param dir2 The directory dir2.
// **/
// int equal_dirs(Directory* dir1, Directory* dir2){
//     if (dir1 == dir2) return 1;
//     else if ((dir1 == NULL) != (dir2 == NULL)) return 0;
//     else if (strcmp(dir1->name, dir2->name) != 0) return 0;
//     else if (!equal_dirs(dir1->first, dir2->first)) return 0;
//     else if (!equal_dirs(dir1->next, dir2->next)) return 0;
//     else return 1;
// }

// /**
// * \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
// * \since 25/03/2023.
// * \brief Function that returns a memory's address to a copy of directory dir.
// * \param dir The directory that will be copied.
// **/
// Directory* copy_dir(Directory* dir){
//     if (dir == NULL) return NULL;
//     Directory *d = (Directory*)malloc(sizeof(Directory));
//     strcpy(d->name, dir->name);
//     d->first = dir->first;
//     d->next = dir->next;
//     return d;
// }
