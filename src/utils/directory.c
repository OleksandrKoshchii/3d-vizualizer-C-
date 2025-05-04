#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdint.h>

#include "directory.h"

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
    directory->active_file = 0;
    
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

directory_t* get_directory_file_names(char* dir_name, char* type) {
    
    DIR* dir = open_directory(dir_name);
    directory_t* directory = initialize_directory();
    
    
    struct dirent* entry;
    while((entry = readdir(dir)) != NULL && directory->file_count < MAX_FILES_SIZE) {
        // Skip "." and ".."
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if(type == NULL || strstr(entry->d_name, type)) {
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

void print_dir_file_names(directory_t* dir) {
    
    int i = 0;
    while (dir->file_names[i] && i < dir->file_count) {
        printf("%s\n", dir->file_names[i]);
        i++;
    }
}

void draw_highlighted_string(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, char* filename, unsigned int color) {
    draw_char_text(buffer, x, y, '-', color); // Optional
    draw_char_text(buffer, x, y, '>', color);
    draw_string(buffer, x + char_width('>') + 5, y, filename, color);
}

void display_files(directory_t* dir, uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    static float global_y = 0;
    
    float file_x = 50;
    float y_indent = 20;
    float spacer = 50;
    
    float active_y = y_indent + dir->active_file * spacer + global_y;

    // Move up if active file is too close to bottom
    while (active_y > SCREEN_HEIGHT - spacer) {
        global_y -= (active_y - (SCREEN_HEIGHT - spacer));
        active_y = y_indent + dir->active_file * spacer + global_y;
    }
    // Move down if file is too close to top
    while (active_y < spacer) {
        global_y += (spacer - active_y);
        active_y = y_indent + dir->active_file * spacer + global_y;
    }
    
    unsigned int col = hsv2rgb_lcd(200, 200, 200);
    unsigned int col_highlighted = hsv2rgb_lcd(0, 0, 255);

    for(int i = 0; i < dir->file_count; i++) {
        float file_y = y_indent + i * spacer + global_y;

        // Check for OOB
        if((file_y > 0 && file_y < SCREEN_HEIGHT)) { // might need to change the edges
            if(dir->active_file == i) {
                draw_highlighted_string(buffer, file_x, file_y, dir->file_names[i], col_highlighted);
            }
            else {
                draw_string(buffer, file_x, file_y, dir->file_names[i], col);
            }
        }
    }    
}

void display_files_centered(directory_t* dir, uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    static float global_y = 0;
    
    float file_x = SCREEN_WIDTH / 2;
    float y_indent = 20;
    float spacer = 50;
    
    float active_y = y_indent + dir->active_file * spacer + global_y;

    // Move up if active file is too close to bottom
    if (active_y > SCREEN_HEIGHT - spacer) {
        global_y -= (active_y - (SCREEN_HEIGHT - spacer));
    }
    // Move down if file is too close to top
    if (active_y < spacer) {
        global_y += (spacer - active_y);
    }
    
    unsigned int col = hsv2rgb_lcd(200, 200, 200);
    unsigned int col_highlighted = hsv2rgb_lcd(0, 0, 255);

    for(int i = 0; i < dir->file_count; i++) {
        float file_y = y_indent + i * spacer + global_y;

        // Check for OOB
        if((file_y > 0 && file_y < SCREEN_HEIGHT)) { // might need to change the edges
            int x_indent = file_x - string_width(dir->file_names[i]) / 2;

            if(dir->active_file == i) {
                draw_highlighted_string(buffer, x_indent, file_y, dir->file_names[i], col_highlighted);
            }
            else {
                draw_string(buffer, x_indent, file_y, dir->file_names[i], col);
            }
        }
    }    
}

void free_directory(directory_t* directory) {

    for (int i = 0; i < directory->file_count; i++) {
        free(directory->file_names[i]);
    }
    free(directory);
}
