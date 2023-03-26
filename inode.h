#ifndef _inode_h
#define _inode_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Inode{
	struct HeadInode * head;
    int direct_data[6];
	struct Indirect *indirect1;
	struct Indirect *indirect2;
	struct Indirect *indirect3;
};

enum FileType {
    REGULAR_FILE = 1,
    DIRECTORY = 2,
    SYMBOLIC_LINK = 3,
    BLOCK_DEVICE = 4,
    CHARACTER_DEVICE = 5,
    PIPE = 6
};

struct HeadInode{
    int file_size;
    enum FileType file_type;
    time_t creation_time;
    time_t modify_time;
};


struct Indirect{
	union data{
		unsigned long int node;
		struct Indirect *indirect;
	};
	struct Indirect *next;
};

#endif