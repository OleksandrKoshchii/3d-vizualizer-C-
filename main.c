/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  main.c      - main file

  include your name there and license for distribution.

  Remove next text: This line should not appear in submitted
  work and project name should be change to match real application.
  If this text is there I want 10 points subtracted from final
  evaluation.

 *******************************************************************/
#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <dirent.h>
#include <stdbool.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include "screen_tools.h"
#include "render.h"
#include "matrix_operations.h"
#include "object_transformations.h"
#include "read_stl.h"

#include "directory.h"
#include "text.h"
#include "knob.h"
#include "font_types.h"

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;
const float FOV = 60;
#define FPS 60



const int MODE_MAX = 1;
//-----------------------------------

char* model = "skull.stl";
#define DOUBLE_CLICK_TOLERANCE 0.2 // Doensn't work
#define CURRENT_DIRECTORY "."
#define FILE_TYPE ".stl"

int state = 0;
font_descriptor_t *fdes;

void draw_pixel(unsigned char *parlcd_mem_base,uint16_t color);
void draw_frame(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH],unsigned char* parlcd_mem_base);
void clear_buffer(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]);
void print_stats(enum Mode mode, int* fps, clock_t* start, knobs_t* knobs);


int main(int argc, char *argv[])
{
    printf("Starting display loop...\n");
	bool running = true;

    unsigned char *mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    unsigned char *parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (!mem_base || !parlcd_mem_base) {
        fprintf(stderr, "Memory mapping failed\n");
        return 1;
    }

	// Initialize knobs
	knobs_t* knobs = initialize_knobs();

    parlcd_hx8357_init(parlcd_mem_base);

    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 1000000000 / FPS;

	camera_t cam = {
		{-3,0,0.5f},
		{
			{0,-1,0},
			{0,0,-1},
			{1,0,0},
		}
	};

	float light[3] = {0.f,0.f,10.f};

	obj_t obj = readBinarySTL(model);
	obj_t objs[1] = {obj,};
	uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	
	clock_t start = clock();
	int fps = 0;
 
	bool chooseMode = false;
	enum Mode mode = POLYGON;

	//-----------------------------------
	directory_t* dir = get_directory_file_names(CURRENT_DIRECTORY, FILE_TYPE);

	fdes = &font_winFreeSystem14x16;

	while (running) {
		switch (state) {
			case 0:
				while (state == 0) {

					read_knobs_values(mem_base, knobs);

					print_dir_file_names(dir);
					// display_files();
					
					clear_buffer(pixelBuffer);
					
					// Doesn't work
					unsigned int col = hsv2rgb_lcd(155, 155, 155);
					draw_char_test(pixelBuffer, 100, 100, 'g', col);
					draw_char_test(pixelBuffer, 200, 200, 'g', col);

					draw_frame(pixelBuffer, parlcd_mem_base);
					
					int i = 0;
					if((i % 100) == 0) printf("State=%d\n", state);
					i++;
					clock_nanosleep(CLOCK_MONOTONIC, 0, &delay, NULL);

					check_state(&state, knobs);
				}
				break;
			case 1:
				while (state == 1) {
					read_knobs_values(mem_base, knobs);
					if(knobs->encodersSwitched[0]) chooseMode = !chooseMode;

					if(chooseMode) {
						if (knobs->encodersDif[0] > 0) {
							mode++;
							if(mode > MODE_MAX) mode = 0;
						} else if(knobs->encodersDif[0] < 0) {
							mode--;
							if(mode < 0) mode = MODE_MAX;
						}
					}

					if(knobs->encodersDif[0] != 0) {
						cam.coord[0] += knobs->encodersDif[0] * 0.05f;
					}
					if(knobs->encodersDif[1] != 0) {
						rotate_obj_horizontal(&obj, knobs->encodersDif[1]);
					}
					if(knobs->encodersDif[2] != 0) {
						rotate_obj_vertical(&obj, knobs->encodersDif[2]);
					}

					print_stats(mode, &fps, &start, knobs);

					inverse(cam.orientation, cam.inv_orientation);
					clear_buffer(pixelBuffer);
					proj_objs(objs, 1, cam, light, pixelBuffer, mode);
					draw_frame(pixelBuffer, parlcd_mem_base);
					// cam.coord[0] += 0.01f;

					clock_nanosleep(CLOCK_MONOTONIC, 0, &delay, NULL);

					check_state(&state, knobs);
				}
				break;
		}
    }

	free_obj(obj);
	free(knobs);
	free_directory(dir);
	munmap_phys_address(mem_base, SPILED_REG_SIZE);
	munmap_phys_address(parlcd_mem_base, PARLCD_REG_SIZE);

    return 0;
}


void draw_pixel(unsigned char *parlcd_mem_base,uint16_t color) {
	parlcd_write_data(parlcd_mem_base, color);
}

void draw_frame(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH],unsigned char* parlcd_mem_base) {
	parlcd_write_cmd(parlcd_mem_base, 0x2C);
	for(int y = 0; y < SCREEN_HEIGHT; y++){
		for(int x = 0; x < SCREEN_WIDTH; x++){
			draw_pixel(parlcd_mem_base,pixelBuffer[y][x]);
		}
	}
}

void print_stats(enum Mode mode, int* fps, clock_t* start, knobs_t* knobs) {
	printf("%d B1: angle-%d pressed-%d switched-%d dif: %d;   B2: angle:%d pressed:%d switched:%d dif: %d;   B3: angle:%d pressed:%d switched:%d dif: %d\n", mode, knobs->encodersValues[0], knobs->encodersPressed[0], knobs->encodersSwitched[0], knobs->encodersDif[0], knobs->encodersValues[1], knobs->encodersPressed[1], knobs->encodersSwitched[1], knobs->encodersDif[1], knobs->encodersValues[2], knobs->encodersPressed[2], knobs->encodersSwitched[2], knobs->encodersDif[2]);
	
	(*fps)++;

	if((clock() - (*start)) / CLOCKS_PER_SEC > 1){
		printf("fps: %d\n", *fps);
		*fps = 0;
		*start = clock();
	}
}

void clear_buffer(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
	for(int i = 0; i < SCREEN_HEIGHT;i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			pixelBuffer[i][j] = 0x00;
		}
	}
}
