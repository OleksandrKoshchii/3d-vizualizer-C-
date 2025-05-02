#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>

#define DIRECTORY_NAME "." // Current directory
#define TYPE ".stl"
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

void draw_filename(char* file_name) {
    for(int i = 0; i < file_name[i] != '\0'; i++) {
        printf("%c ", file_name[i]);
    }
}

int main(int argc, char* argv[]) {
    
    DIR* dir = open_directory(DIRECTORY_NAME);
    directory_t* directory = initialize_directory();

    get_directory_file_names(dir, directory, TYPE);
    
    print_dir_file_names(dir, directory);

    
    closedir(dir);
    free_directory(directory);

    return EXIT_SUCCESS;
}

void get_directory_file_names(DIR* dir, directory_t* directory, char* type) {
    
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
}

void print_dir_file_names(DIR* dir, directory_t* directory) {
    
    int i = 0;
    while (directory->file_names[i] && i < directory->file_count) {
        printf("%s\n", directory->file_names[i]);
        i++;
    }
}

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

void free_directory(directory_t* directory) {

    for (int i = 0; i < directory->file_count; i++) {
        free(directory->file_names[i]);
    }
    free(directory);
}