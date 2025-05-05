/*******************************************************************
  Main function of the 3D visualizer application

  main.c      - main file

  include your name there and license for distribution.

  After startup the user is presented with a screen where they can
  choose a 3D model to be loaded in, the model can be interacted with 
  using knobs.

 *******************************************************************/
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include "matrix_operations.h"
#include "font_types.h"
#include "directory.h"
#include "main_utils.h"
#include "text.h"
#include "knob.h"

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;
const float FOV = 60;
const int MODE_MAX = 1;
const int menu_scale = 4;
const int viewer_scale = 2;
#define FPS 60

//-----------------------------------

char* current_object = "";
#define CURRENT_DIRECTORY "."
#define FILE_TYPE ".stl"

int state = 0;
font_descriptor_t *fdes;

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

	struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 1000000000 / FPS;
	uint16_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	
	clock_t start = clock();
	int fps = 0;
	
	bool choose_mode = false;
	enum Mode mode = POLYGON;
	//-----------------------------------
	parlcd_hx8357_init(parlcd_mem_base);
	
	knobs_t* knobs = initialize_knobs();
    
	directory_t* dir = get_directory_file_names(CURRENT_DIRECTORY, FILE_TYPE);
	
	camera_t cam = initialize_camera();
	
	float light[3] = {0.f,0.f,10.f};
	
	fdes = &font_winFreeSystem14x16;
	// fdes = &font_rom8x16;
	
	obj_t obj = {0};
	obj_t objs[1];
	char* object_to_load = dir->file_names[dir->active_file];

	while (running) {
		switch (state) {
			case 0:
				while (state == 0) {
					// Main menu loop

					read_knobs_values(mem_base, knobs);
					
					clear_buffer(pixel_buffer);
					
					// display_files(dir, pixel_buffer, menu_scale);
					display_files_centered(dir, pixel_buffer, menu_scale);
					
					choose_file(knobs, dir);
					object_to_load = dir->file_names[dir->active_file];

					draw_frame(pixel_buffer, parlcd_mem_base);
					
					// print_stats(mode, &fps, &start, knobs);
					
					clock_nanosleep(CLOCK_MONOTONIC, 0, &delay, NULL);
					
					switch_state(&state, knobs);
				}
				break;
			case 1:
				while (state == 1) {
					// 3D model viewing loop

					if(strcmp(current_object, object_to_load) != 0) {
						// Load a new object
						free_obj(&obj);
						obj = load_object(object_to_load);
						objs[0] = obj;
						current_object = object_to_load;
					}

					read_knobs_values(mem_base, knobs);
					if(knobs->encoders_switched[0]) choose_mode = !choose_mode;


					check_mode(&mode, choose_mode, knobs);

					check_rotation(&obj, &cam, knobs);
					
					clear_buffer(pixel_buffer);

					print_stats(mode, &fps, &start, knobs);
					
					inverse(cam.orientation, cam.inv_orientation);
					proj_objs(objs, 1, cam, light, pixel_buffer, mode);
					
					draw_fps(pixel_buffer, &fps, viewer_scale);
					draw_frame(pixel_buffer, parlcd_mem_base);
					// cam.coord[0] += 0.01f;

					clock_nanosleep(CLOCK_MONOTONIC, 0, &delay, NULL);

					switch_state(&state, knobs);
				}
				break;
		}
    }

	free(knobs);
	free_obj(&obj);
	free_directory(dir);
    
	return 0;
}
