/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 24/04/2023.
**/

#ifndef _file_operations_h
#define _file_operations_h
#include "file_operations.h"

void allocate_dir(int fdHd, struct SuperBlock ReadBlock, char *dir_name);
long int find_dir(int fdHd, struct SuperBlock ReadBlock,long int father_address, char *dir_name);
void show_all_directories(int fdHd, struct SuperBlock ReadBlock);
#endif