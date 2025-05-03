#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "directory.h"

#define MAX_FILES_SIZE 20


directory_t* initialize_directory() {
    
    directory_t* directory;
    directory = malloc(sizeof(directory_t));
    if (directory == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < MAX_FILES_SIZE; i++) {
        directory->file_names[i] = NULL;
    }
    directory->file_count = 0;
    
    return directory;
}

DIR* open_directory(char* dir_name) {

    DIR* dir = opendir(dir_name);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    return dir;
}

void draw_filename(char* file_name) {
    for(int i = 0; file_name[i] != '\0'; i++) {
        printf("%c ", file_name[i]);
    }
}

directory_t* get_directory_file_names(char* dir_name, char* type) {
    
    DIR* dir = open_directory(dir_name);
    directory_t* directory = initialize_directory();
    
    if(type == NULL) {
        type = ".stl";
    }

    struct dirent* entry;
    while((entry = readdir(dir)) != NULL && directory->file_count < MAX_FILES_SIZE) {

        if(strstr(entry->d_name, type)) {
            directory->file_names[directory->file_count] = malloc(strlen(entry->d_name) + 1);
            if (directory->file_names[directory->file_count] == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            strcpy(directory->file_names[directory->file_count], entry->d_name);
            directory->file_count++;
        }
    }
    closedir(dir);

    return directory;
}

void print_dir_file_names(directory_t* directory) {
    
    int i = 0;
    while (directory->file_names[i] && i < directory->file_count) {
        printf("%s\n", directory->file_names[i]);
        i++;
    }
}

void free_directory(directory_t* directory) {

    for (int i = 0; i < directory->file_count; i++) {
        free(directory->file_names[i]);
    }
    free(directory);
}
