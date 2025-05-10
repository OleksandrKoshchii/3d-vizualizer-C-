#ifndef MAIN_UTILS_H
#define MAIN_UTILS_H

#include <stdint.h>
#include <time.h>
#include "read_stl.h"
#include "knob.h"
#include "render.h"

extern char* current_object;

obj_t load_object(char* path, char* object_to_load);

void draw_pixel(unsigned char *parlcd_mem_base, uint16_t color);

void draw_frame(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], unsigned char* parlcd_mem_base);

void clear_buffer(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH]);

void print_stats(int* fps, clock_t* start, knobs_t* knobs);

void draw_fps(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], clock_t* start, int* fps, int scale);

void draw_mode(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], bool wireframe, int scale);


void draw_RGB_stats(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int scale);

void switch_mode(knobs_t* knobs);

void change_color(knobs_t* knobs);

void change_light_position(knobs_t* knobs, float light[3]);

camera_t initialize_camera();

#endif /* MAIN_UTILS_H */
