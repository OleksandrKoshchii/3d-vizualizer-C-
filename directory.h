#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>

#define MAX_FILES_SIZE 20

typedef struct directory_t {
    char* file_names[MAX_FILES_SIZE];
    int file_count;
} directory_t;

directory_t* get_directory_file_names(char* dir_name, char* type);

void print_dir_file_names(directory_t* directory);

void free_directory(directory_t* directory);

#endif /* DIRECTORY_H */