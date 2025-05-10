/*******************************************************************
  Main function of the 3D visualizer application

  main.c - main file

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

const float FOV = 60;
const int menu_scale = 4;
const int viewer_scale = 2;
#define FPS 60

//.....drawing properties.....//
bool wireframe = false;
uint8_t RED = 255;
uint8_t GREEN = 0;
uint8_t BLUE = 0;
//.....drawing properties.....//

//.....MODES.....//
bool changing_color = false;
bool changing_light_pos = false;
//.....MODES.....//

char* current_object = "";
#define CURRENT_DIRECTORY "/tmp/models"
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
	
	//-----------------------------------
	parlcd_hx8357_init(parlcd_mem_base);
	
	knobs_t* knobs = initialize_knobs();
    
	directory_t* dir = get_directory_file_names(CURRENT_DIRECTORY, FILE_TYPE);
	
	camera_t cam = initialize_camera();
	
	float light[3] = {0.f,0.f,10.f};
	
	fdes = &font_winFreeSystem14x16;
	// fdes = &font_rom8x16;
	
	obj_t obj = {0};
	char* object_to_load = dir->file_names[dir->active_file];

	while (running) {
		switch (state) {
			case 0:
				while (state == 0) {
					// Main menu loop
					clear_buffer(pixel_buffer);

					read_knobs_values(mem_base, knobs);
					
					display_files_centered(dir, pixel_buffer, menu_scale);
					
					choose_file(knobs, dir);
					object_to_load = dir->file_names[dir->active_file];

					draw_frame(pixel_buffer, parlcd_mem_base);
					
					// print_stats(mode, &fps, &start, knobs);
					
					clock_nanosleep(CLOCK_MONOTONIC, 0, &delay, NULL);
					
					if(check_shutdown(&running, knobs)) {
						clear_buffer(pixel_buffer);
						draw_frame(pixel_buffer, parlcd_mem_base);
						break;
					}
						
					switch_state(&state, knobs);
				}
				break;
			case 1:
				while (state == 1) {
					// 3D model viewing loop
					clear_buffer(pixel_buffer);

					if(strcmp(current_object, object_to_load) != 0) {
						// Load a new object
						free_obj(&obj);
						obj = load_object(CURRENT_DIRECTORY, object_to_load);
						current_object = object_to_load;
					}

					read_knobs_values(mem_base, knobs);

					switch_mode(knobs);

					if(changing_color){
						change_color(knobs);
						draw_RGB_stats(pixel_buffer, viewer_scale);
					}else if(changing_light_pos){
						change_light_position(knobs, light);
						draw_light_position(pixel_buffer, light, viewer_scale);
					}else{
						check_rotation(&obj, &cam, knobs);
					}

					inverse(cam.orientation, cam.inv_orientation);
					
					proj_obj(&obj, cam, light, pixel_buffer);
					
					draw_fps(pixel_buffer, &start, &fps, viewer_scale);
					draw_mode(pixel_buffer, wireframe, viewer_scale);

					print_stats(&fps, &start, knobs);

					draw_frame(pixel_buffer, parlcd_mem_base);

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
