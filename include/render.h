#ifndef __RENDER__
#define __RENDER__

#include <stdint.h>

#include "read_stl.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
enum Mode {
	POLYGON,
	WIREFRAME
};

typedef struct{
	float coord[3];
	float orientation[3][3];
	float inv_orientation[3][3];
} camera_t;

void proj_triangle(triangle_t triangle,camera_t cam,float light[3],bool shadow,uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH],enum Mode mode);

triangle_t world_to_camera(triangle_t worldTriangle,camera_t cam);

void proj_objs(obj_t* objs,int obj_quantity,camera_t cam,float light[3],uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH],enum Mode mode);
#endif
