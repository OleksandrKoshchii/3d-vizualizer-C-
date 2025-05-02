#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>

#define MAX_FILES_SIZE 20

typedef struct directory_t {
    char* file_names[MAX_FILES_SIZE];
    int file_count;
} directory_t;

void get_directory_file_names(DIR* dir, directory_t* directory, char* type);
void print_dir_file_names(DIR* dir, directory_t* directory);
DIR* open_directory(char* dir_name);
directory_t* initialize_directory();
void free_directory(directory_t* directory);
