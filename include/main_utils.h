#ifndef MAIN_UTILS_H
#define MAIN_UTILS_H

#include <stdint.h>
#include <time.h>
#include "read_stl.h"
#include "knob.h"

extern const int SCREEN_HEIGHT;
extern const int SCREEN_WIDTH;
extern char* current_object;

obj_t load_object(char* object_to_load);

void draw_pixel(unsigned char *parlcd_mem_base, uint16_t color);

void draw_frame(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH], unsigned char* parlcd_mem_base);

void clear_buffer(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]);

void print_stats(enum Mode mode, int* fps, clock_t* start, knobs_t* knobs);

camera_t initialize_camera();

#endif /* MAIN_UTILS_H */