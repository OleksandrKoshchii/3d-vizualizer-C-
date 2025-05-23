#ifndef __RENDER__
#define __RENDER__

#include <stdint.h>
#include <stdbool.h>

#include "read_stl.h"

#define SCREEN_HEIGHT 320
#define SCREEN_WIDTH 480

typedef struct{
	float coord[3];
	float orientation[3][3];
	float inv_orientation[3][3];
} camera_t;

void proj_triangle(triangle_t triangle,camera_t cam,float light[3],bool shadow,uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]);

triangle_t world_to_camera(triangle_t worldTriangle,camera_t cam);

void proj_obj(obj_t* obj,camera_t cam,float light[3],uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]);
#endif
