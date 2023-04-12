/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
**/

#ifndef _directory_h
#define _directory_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Struct representing a directory hierarchy. Using the data structure known as multiway trees as a basis. 
**/
typedef struct directory {
	char name[60]; // Name of the directory.
	int inode; // Inode number of the directory.
	struct directory *first; // Pointer to the first child directory.
	struct directory *next; // Pointer to the next sibling directory.
} Directory;

Directory* create_dir(char* _name, int _inode);
void dir_insert(Directory* dir, Directory* sub_dir);
void ls_dir(Directory* dir);
void print_tree(Directory* dir);
int dir_pertain(Directory* dir, char* name);
void release_dir(Directory* dir);
int number_of_leafs(Directory* dir);
int dir_height(Directory* dir);
int equal_dirs(Directory* dir1, Directory* dir2);
Directory* copy_dir(Directory* dir);

#endif