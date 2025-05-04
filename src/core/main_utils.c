#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "knob.h"
#include "render.h"
#include "read_stl.h"
#include "mzapo_parlcd.h"
#include "main_utils.h"

obj_t load_object(char* object_to_load) {
    printf("Loading 3D model: %s\n", object_to_load);
    obj_t obj = readBinarySTL(object_to_load);
    current_object = object_to_load;
    
    return obj;
}

void draw_pixel(unsigned char *parlcd_mem_base, uint16_t color) {
	parlcd_write_data(parlcd_mem_base, color);
}

void draw_frame(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH], unsigned char* parlcd_mem_base) {
	parlcd_write_cmd(parlcd_mem_base, 0x2C);
	for(int y = 0; y < SCREEN_HEIGHT; y++) {
		for(int x = 0; x < SCREEN_WIDTH; x++) {
			draw_pixel(parlcd_mem_base,pixelBuffer[y][x]);
		}
	}
}

void clear_buffer(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
	for(int i = 0; i < SCREEN_HEIGHT;i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			pixelBuffer[i][j] = 0x00;
		}
	}
}

void print_stats(enum Mode mode, int* fps, clock_t* start, knobs_t* knobs) {
	printf("%d B1: angle-%d pressed-%d switched-%d dif: %d; "
			  "B2: angle:%d pressed:%d switched:%d dif: %d; "
			  "B3: angle:%d pressed:%d switched:%d dif: %d\n", 
		       mode, knobs->encoders_values[0], knobs->encoders_pressed[0], knobs->encoders_switched[0], knobs->encoders_diff[0], 
			   knobs->encoders_values[1], knobs->encoders_pressed[1], knobs->encoders_switched[1], knobs->encoders_diff[1], 
			   knobs->encoders_values[2], knobs->encoders_pressed[2], knobs->encoders_switched[2], knobs->encoders_diff[2]);
	
	(*fps)++;

	if((clock() - (*start)) / CLOCKS_PER_SEC > 1) {
		printf("fps: %d\n", *fps);
		*fps = 0;
		*start = clock();
	}
}

camera_t initialize_camera() {
    camera_t cam = {
		{-3,0,0.5f},
		{
			{0,-1,0},
			{0,0,-1},
			{1,0,0},
		}
	};

    return cam;
}
