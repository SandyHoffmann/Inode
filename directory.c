/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
**/

#include "directory.h"

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that creates a directory, represented by a leaf in a multiway tree. The address of it in the memory is returned.
* \param _name The directory's name.
* \param _inode The inode's number.
**/
Directory* create_dir(char* _name, int _inode){
    // Allocate memory for the directory structure
    Directory* dir = (Directory*)malloc(sizeof(Directory));
    // Copy the directory name to the directory structure
    strcpy(dir->name, _name);
    // Set the inode number of the directory
    dir->inode = _inode;
    // Set the first child of the directory to NULL
    dir->first = NULL;
    // Set the next sibling of the directory to NULL
    dir->next = NULL;
    // Return the pointer to the created directory
    return dir;
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that inserts a directory into another directory, making it a subdirectory of the latter.
* \param dir The parent directory.
* \param sub_dir The directory that'll be inserted as a subdirectory of dir.
**/
void dir_insert(Directory* dir, Directory* sub_dir){
    sub_dir->next = dir->first;
    dir->first = sub_dir;
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that lists the names of all subdirectories of a given directory. Basically it's a command similar to 'ls'.
* \param dir The directory where the list command will be executed.
**/
void ls_dir(Directory* dir){
    Directory *d = dir->first;
    /*
    An iteration is executed through its linked list of subdirectories (pointed by the "first" pointer). For each subdirectory, its name is printed.
    */
    while (d != NULL){
        printf("\n%s", d->name);
        d = d->next;
    }
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that prints (in Preorder) all the directory hierarchy of the given directory and its subdirectories.
* \param dir The root directory where the print starts.
**/
void print_tree(Directory* dir){
    printf("\n%s", dir->name);
    Directory *d = dir->first;

    while (d != NULL){
        // Recursively call print_tree() for all subdirectories.
        print_tree(d);
        d = d->next;
    }
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that returns 1 if a directory pertains to a given directory hierarchy, starting from a root directory, or 0 if not.
* \param dir The root directory where the search will be executed.
* \param _name The directory's name that'll be searched.
**/
int dir_pertain(Directory* dir, char* _name){
    // If the directory is null, it doesn't pertain to the hierarchy.
    if (dir == NULL) return 0;
    // Check if the current directory has the given name.
    int equal = strcmp(dir->name, _name);

    // Recursive search in the current directory's subdirectories and siblings. If it finds, returns 1, otherwise returns 0.
    return (!equal) || (dir_pertain(dir->first, _name)) || (dir_pertain(dir->next, _name));
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that releases the allocated memory of the tree's directory. 
* \param dir The root directory itself.
**/
void release_dir(Directory* dir){
    // Checks if directory exists.
    if (dir != NULL){
        // Recursively calls the function to release the memory of the first child directory.
        release_dir(dir->first);
        // Recursively calls the function to release the memory of the next sibling directory.
        release_dir(dir->next);
        // Frees the memory allocated for the current directory.
        free(dir);
    } 
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that returns the number of leafs, i.e, the number of files or directories without subdirectories or files.  
* \param dir The root directory itself.
**/
int number_of_leafs(Directory* dir){
    if (dir == NULL) return 0;
    int leafs = 0;
    Directory* children_dir = dir->first;

    while (children_dir != NULL){
        Directory* sis_dir = children_dir->next;

        while (sis_dir != NULL){
            leafs += number_of_leafs(sis_dir);
            sis_dir = sis_dir->next;
        }

        children_dir = children_dir->first;
    }

    return ++leafs; 
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that returns the tree's height of the directories hierarchy. It basically computes recursively the height of the directory hierarchy by traversing the directories
and its subdirectories. For each directory, it calls itself recursively, and stores the maximum height. The height is incremented by 1 after the last recursive call.
* \param dir The root directory itself.
**/
int dir_height(Directory* dir){
    // Initializes the maximum height with -1.
    int hMax = -1;
    Directory *d = dir->first;

    while (d != NULL){
        // Recursively computes the height of the subdirectory.
        int h = dir_height(d);
        // Updates the maximum height if necessary.
        if (h > hMax) hMax = h;
        d = d->next;
    }

    // Increments the height by 1 and returns.
    return ++hMax;
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that returns 1 if directory dir1 is equal to directory dir2, or 0 if not.  
* \param dir1 The directory dir1.
* \param dir2 The directory dir2.
**/
int equal_dirs(Directory* dir1, Directory* dir2){
    // If the directories have the same memory address, they are the same.
    if (dir1 == dir2) return 1;

    // If one directory is null and the other is not, they are not equal.
    else if ((dir1 == NULL) != (dir2 == NULL)) return 0;

    // If the directories have different names, they are not equal.
    else if (strcmp(dir1->name, dir2->name) != 0) return 0;

    // Recursively check if the subdirectories are equal.
    else if (!equal_dirs(dir1->first, dir2->first)) return 0;

    // Recursively check if the next directories are equal.
    else if (!equal_dirs(dir1->next, dir2->next)) return 0;

    // If none of the above conditions are met, the directories are equal.  
    else return 1;
}

/**
* \authors Sandy Hoffmann and Leonardo de Souza Fiamoncini.
* \since 25/03/2023.
* \brief Function that returns a pointer to the memory's address where the copy of directory dir os located.  
* \param dir The directory that will be copied.
**/
Directory* copy_dir(Directory* dir){
    // If dir is null, return null.
    if (dir == NULL) return NULL;

    // Allocate memory for the new directory.
    Directory *d = (Directory*)malloc(sizeof(Directory));

    // Copy the name of the original directory to the new one.
    strcpy(d->name, dir->name);

    // Set the first child of the new directory to be the same as the original directory's first child.
    d->first = dir->first;

    // Set the next sibling of the new directory to be the same as the original directory's next sibling.
    d->next = dir->next;

    // Return a pointer to the new directory.
    return d;
}