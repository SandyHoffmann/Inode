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
 * \update first and next will keep the int address of the first and next directory in the hierarchy. (struct just to manipulate data)
 **/
typedef struct directory
{
	char name[60];
	int inode;
	unsigned long int first_int;
	struct directory *first_structure;
	unsigned long int next_int;
	struct directory *next_structure;
} Directory;

Directory *create_dir(char *_name, int _inode);

#endif