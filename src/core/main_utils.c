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


//.....drawing properties.....//
extern uint16_t RED;
extern uint16_t GREEN;
extern uint16_t BLUE;
//.....drawing properties.....//

obj_t load_object(char* path, char* object_to_load) {
    char absolute_path[256];
	snprintf(absolute_path, sizeof(absolute_path), "%s/%s", path, object_to_load);

	printf("Loading 3D model: %s\n", absolute_path);
    obj_t obj = readBinarySTL(absolute_path);
    current_object = object_to_load;

    return obj;
}

void draw_pixel(unsigned char *parlcd_mem_base, uint16_t color) {
	parlcd_write_data(parlcd_mem_base, color);
}

void draw_frame(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], unsigned char* parlcd_mem_base) {
	parlcd_write_cmd(parlcd_mem_base, 0x2C);
	for(int y = 0; y < SCREEN_HEIGHT; y++) {
		for(int x = 0; x < SCREEN_WIDTH; x++) {
			draw_pixel(parlcd_mem_base,pixel_buffer[y][x]);
		}
	}
}

void clear_buffer(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
	for(int i = 0; i < SCREEN_HEIGHT;i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			pixel_buffer[i][j] = 0x00;
		}
	}
}

void print_stats(int* fps, clock_t* start, knobs_t* knobs) {
	printf("B1: angle-%d pressed-%d switched-%d dif: %d; "
			  "B2: angle:%d pressed:%d switched:%d dif: %d; "
			  "B3: angle:%d pressed:%d switched:%d dif: %d\n", knobs->encoders_values[0], knobs->encoders_pressed[0], knobs->encoders_switched[0], knobs->encoders_diff[0], 
			   knobs->encoders_values[1], knobs->encoders_pressed[1], knobs->encoders_switched[1], knobs->encoders_diff[1], 
			   knobs->encoders_values[2], knobs->encoders_pressed[2], knobs->encoders_switched[2], knobs->encoders_diff[2]);
	
	(*fps)++;

	if((clock() - (*start)) / CLOCKS_PER_SEC > 1) {
		printf("fps: %d\n", *fps);
		*fps = 0;
		*start = clock();
	}
}

void draw_fps(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], clock_t* start, int* fps, int scale) {
	static char fps_str[20] = "--";
	if((clock() - (*start)) / CLOCKS_PER_SEC > 1) {
		// Needs a fix
		snprintf(fps_str, sizeof(fps_str), "FPS: %d", *fps);
	}
	unsigned int col = hsv2rgb_lcd(0, 0, 255);
	draw_string(pixel_buffer, 5, 295, fps_str, col, scale);
}

void draw_mode(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], bool wireframe, int scale) {
	unsigned int col = hsv2rgb_lcd(0, 0, 255);
	if(wireframe){
		draw_string(pixel_buffer, 5, 5, "WIREFRAME", col, scale);
	} else {
		draw_string(pixel_buffer, 5, 5, "POLYGON", col, scale);
	}
}

void draw_RGB_stats(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int scale){
	unsigned int col = hsv2rgb_lcd(0, 0, 255);
	draw_string(pixel_buffer, 370, 5, "COLOR:", col, scale);

	char str[20];
	snprintf(str, 20, "R: %hhu", RED);
	draw_string(pixel_buffer, 370, 30, str, col, scale);
	snprintf(str, 20, "G: %hhu", GREEN);
	draw_string(pixel_buffer, 370, 55, str, col, scale);
	snprintf(str, 20, "B: %hhu", BLUE);
	draw_string(pixel_buffer, 370, 80, str, col, scale);
}

void draw_light_position(uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH], float light[3], int scale){
	unsigned int col = hsv2rgb_lcd(0, 0, 255);
	draw_string(pixel_buffer, 370, 5, "LIGHT:", col, scale);

	char str[20];
	snprintf(str, 20, "X: %.0f", light[0]);
	draw_string(pixel_buffer, 370, 30, str, col, scale);
	snprintf(str, 20, "Y: %.0f", light[1]);
	draw_string(pixel_buffer, 370, 55, str, col, scale);
	snprintf(str, 20, "Z: %.0f", light[2]);
	draw_string(pixel_buffer, 370, 80, str, col, scale);
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
