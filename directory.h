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
	char name[60];
	int inode;
	struct directory *first;
	struct directory *next;
} Directory;

Directory* create_dir(char* _name, int _inode);
void dir_insert(Directory* dir, Directory* sub_dir);
void ls_dir(Directory* dir);
void print_root(Directory* dir);
int dir_pertain(Directory* dir, char* name);
void release_dir(Directory* dir);
int number_of_leafs(Directory* dir);
int dir_height(Directory* dir);
int equal_dirs(Directory* dir1, Directory* dir2);
Directory* copy_dir(Directory* dir);

#endif