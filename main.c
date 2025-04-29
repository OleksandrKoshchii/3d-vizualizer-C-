/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

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

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "screen_tools.h"
#include "render.h"
#include "matrix_operations.h"
#include "object_transformations.h"
#include "read_stl.h"


const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;
const float FOV = 60;
#define FPS 60

uint32_t rgb_knobs_value;
signed char enkodersValues[3];
signed char enkodersDif[3];
bool enkodersPressed[3];
bool enkodersSwitched[3];

const int MODE_MAX = 1;


void read_knobs_values(unsigned char* mem_base){
	rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
	for(int enkoder = 0; enkoder < 3; enkoder++){
		
		signed char lastValue = enkodersValues[enkoder];
		bool lastStatus = enkodersPressed[enkoder];

		enkodersValues[enkoder] = (rgb_knobs_value >> 8 * enkoder) & 0xFF;
		enkodersPressed[enkoder] = ((rgb_knobs_value >> 24) >> enkoder) & 0x01 == 0x01 ? true : false;
		enkodersSwitched[enkoder] = lastStatus - enkodersPressed[enkoder] == 1? true : false;

		enkodersDif[enkoder] = enkodersValues[enkoder] - lastValue;
		
		
	}
	
}

void draw_pixel(unsigned char *parlcd_mem_base,uint16_t color){
	parlcd_write_data(parlcd_mem_base,color);
}

draw_frame(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH],unsigned char* parlcd_mem_base){
	parlcd_write_cmd(parlcd_mem_base, 0x2C);
	for(int y = 0; y < SCREEN_HEIGHT; y++){
		for(int x = 0; x < SCREEN_WIDTH; x++){
			draw_pixel(parlcd_mem_base,pixelBuffer[y][x]);
		}
	}
}

void clear_buffer(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]){
	for(int i = 0; i < SCREEN_HEIGHT;i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			pixelBuffer[i][j] = 0x00;
		}
	}
}

int main(int argc, char *argv[])
{
    printf("Starting display loop...\n");

    unsigned char *mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    unsigned char *parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    if (!mem_base || !parlcd_mem_base) {
        fprintf(stderr, "Memory mapping failed\n");
        return 1;
    }

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

	obj_t obj = readBinarySTL("skull.stl");
	obj_t objs[1] = {obj,};
	uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	
	clock_t start = clock();
	int fps = 0;
 
	bool chooseMode = false;

	enum Mode mode = POLYGON;

    while (1) {
		read_knobs_values(mem_base);
		if(enkodersSwitched[0]) chooseMode = !chooseMode;

		if(chooseMode){
			if(enkodersDif[0] > 0){
				mode++;
				if(mode > MODE_MAX) mode = 0;
			}else if(enkodersDif[0] < 0){
				mode--;
				if(mode < 0) mode = MODE_MAX;
			}
			continue;
		}


		if(enkodersDif[0] != 0){
			cam.coord[0] += enkodersDif[0] * 0.05f;
		}
		if(enkodersDif[1] != 0){
			rotate_obj_horizontal(&obj,enkodersDif[1]);
		}
		if(enkodersDif[2] != 0){
			rotate_obj_vertical(&obj,enkodersDif[2]);
		}

		printf("%d button1:%d %d %d dif: %d;   button 2:%d %d %d dif: %d;  button3:%d %d %d dif: %d\n",mode,enkodersValues[0],enkodersPressed[0],enkodersSwitched[0],enkodersDif[0],enkodersValues[1],enkodersPressed[1],enkodersSwitched[1],enkodersDif[1],enkodersValues[2],enkodersPressed[2],enkodersSwitched[2],enkodersDif[2]);
		

		inverse(cam.orientation,cam.inv_orientation);
		clear_buffer(pixelBuffer);
		proj_objs(objs,1,cam,light,pixelBuffer,mode);
		draw_frame(pixelBuffer,parlcd_mem_base);
		//cam.coord[0] += 0.01f;
		fps++;
		if((clock() - start)/CLOCKS_PER_SEC > 1){
			printf("fps: %d\n",fps);
			fps = 0;
			start = clock();
		}
        clock_nanosleep(CLOCK_MONOTONIC, 0, &delay, NULL);
    }

    return 0;
}

