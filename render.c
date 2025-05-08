#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "render.h"
#include "screen_tools.h"
#include "matrix_operations.h"
#include "read_stl.h"

#define ERROR_MALLOC -1
#define RGB 3
#define VERTICES_QUANTITY 3

extern const float FOV;

//.....drawing properties.....//
extern bool wireframe;
extern uint8_t RED;
extern uint8_t GREEN;
extern uint8_t BLUE;
//.....drawing properties.....//

void draw_line(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        if (x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT)
            pixelBuffer[y0][x0] = color;
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}


void draw_triangle(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) {
	draw_line(pixelBuffer, x1, y1, x2, y2,color);
	draw_line(pixelBuffer, x2, y2, x3, y3,color);
	draw_line(pixelBuffer, x3, y3, x1, y1,color);
}

float z_interpolation(float x0, float  y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float x, float y){

	float denom = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2);

	float a = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2))/denom;
	float b = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2))/denom;
	float c = 1.f - a - b;

	return 1.f/(a * (1.f/z0) + b * (1.f/z1) + c * (1.f/z2));
}

void draw_filled_triangle(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    // y sorting
    if (y1 < y0) { int t = y0; y0 = y1; y1 = t; t = x0; x0 = x1; x1 = t;}
    if (y2 < y0) { int t = y0; y0 = y2; y2 = t; t = x0; x0 = x2; x2 = t;}
    if (y2 < y1) { int t = y1; y1 = y2; y2 = t; t = x1; x1 = x2; x2 = t;}

    float inv_slope1, inv_slope2;
    int y;

    if (y1 - y0 != 0) {
        inv_slope1 = (float)(x1 - x0) / (y1 - y0);
        inv_slope2 = (float)(x2 - x0) / (y2 - y0);
        float curx0 = x0;
        float curx1 = x0;

        for (y = y0; y <= y1; y++) {
            int x_start = (int)curx0;
            int x_end = (int)curx1;
            if (x_start > x_end) { int t = x_start; x_start = x_end; x_end = t; }
            for (int x = x_start; x <= x_end; x++) {
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT){

					//float Zvalue = z_interpolation(transformed->vertex[0][0], transformed->vertex[0][1],z0,transformed->vertex[1][0],transformed->vertex[1][1],z1,transformed->vertex[2][0],transformed->vertex[2][1],z2,x,y);
				//	float Zvalue = z_interpolation(x0, y0,z0, x1, y1,z1,x2,y2,z2,x,y);
				//	if(Zvalue <= Zbuffer[y][x]){
				//		Zbuffer[y][x] = Zvalue;
						pixelBuffer[y][x] = color;
				//	}

				}
            }
            curx0 += inv_slope1;
            curx1 += inv_slope2;
        }
    }

    if (y2 - y1 != 0) {
        inv_slope1 = (float)(x2 - x1) / (y2 - y1);
        inv_slope2 = (float)(x2 - x0) / (y2 - y0);
        float curx0 = x1;
        float curx1 = x0 + inv_slope2 * (y1 - y0);

        for (y = y1; y <= y2; y++) {
            int x_start = (int)curx0;
            int x_end = (int)curx1;
            if (x_start > x_end) { int t = x_start; x_start = x_end; x_end = t; }
            for (int x = x_start; x <= x_end; x++) {
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT){
					//float Zvalue = z_interpolation(transformed->vertex[0][0], transformed->vertex[0][1],z0,transformed->vertex[1][0],transformed->vertex[1][1],z1,transformed->vertex[2][0],transformed->vertex[2][1],z2,x,y);
				//	float Zvalue = z_interpolation(x0, y0,z0, x1, y1,z1,x2,y2,z2,x,y);
				//	if(Zvalue <= Zbuffer[y][x]){
				//		Zbuffer[y][x] = Zvalue;
						pixelBuffer[y][x] = color;
				//	}
				}
            }
            curx0 += inv_slope1;
            curx1 += inv_slope2;
        }
    }
}

triangle_t world_to_camera(triangle_t worldTriangle, camera_t cam) {
    triangle_t result;

    for (int vertex = 0; vertex < VERTICES_QUANTITY; vertex++) {
        float copy[RGB];
        for (int i = 0; i < RGB; i++)
            copy[i] = worldTriangle.vertex[vertex][i] - cam.coord[i];

        multiply_mat_vec(copy, cam.inv_orientation);
        for (int i = 0; i < RGB; i++)
            result.vertex[vertex][i] = copy[i];
    }

    for (int i = 0; i < 3; i++) result.normal[i] = worldTriangle.normal[i];

    return result;
}

bool vec_triangle_collided(triangle_t triangle, float point[3], float direction[3], float* t_out) {
    float edge1[3], edge2[3], h[3], s[3], q[3];
    float a, f, u, v, t;

    diff_vectors(triangle.vertex[1], triangle.vertex[0], edge1);
    diff_vectors(triangle.vertex[2], triangle.vertex[0], edge2);

    vec_multiply(direction, edge2, h);
    a = scalar_multiply(edge1, h);

    if (fabs(a) < FLT_EPSILON) return false;

    f = 1.0f / a;
    diff_vectors(point, triangle.vertex[0], s);
    u = f * scalar_multiply(s, h);
    if (u < 0.0f || u > 1.0f) return false;

    vec_multiply(s, edge1, q);
    v = f * scalar_multiply(direction, q);
    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * scalar_multiply(edge2, q);
    if (t > 0.0001f) {
        if (t_out) *t_out = t;
        return true;
    }

    return false;
}

void proj_triangle(triangle_t triangle, camera_t cam, float light[3], bool shadow, uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    float center[3];
    for (int i = 0; i < 3; i++) {
        center[i] = (triangle.vertex[0][i] + triangle.vertex[1][i] + triangle.vertex[2][i]) / 3.f;
    }

    float light_dir[3] = { center[0] - light[0], center[1] - light[1], center[2] - light[2] };
    float cam_dir[3] = { center[0] - cam.coord[0], center[1] - cam.coord[1], center[2] - cam.coord[2] };

    if (scalar_multiply(triangle.normal, cam_dir) >= 0 && !wireframe)return;

    float sc_multiply = scalar_multiply(triangle.normal, light_dir);
    sc_multiply = -sc_multiply / (sqrt(scalar_multiply(light_dir, light_dir)) * sqrt(scalar_multiply(triangle.normal, triangle.normal)));
    if (sc_multiply < 0) sc_multiply = 0;
    sc_multiply = sc_multiply * 0.6f + 0.4f;

    triangle_t transformed = world_to_camera(triangle, cam);

    float aspect = (float)SCREEN_HEIGHT / SCREEN_WIDTH;
    float fov_def = 1.f / tanf((FOV / 2.f) * (M_PI / 180.f));

    int coordX[3], coordY[3];
    for (int i = 0; i < 3; i++) {
        coordX[i] = to_x_coord((transformed.vertex[i][0] / transformed.vertex[i][2]) * aspect * fov_def);
        coordY[i] = to_y_coord((transformed.vertex[i][1] / transformed.vertex[i][2]) * fov_def);
    }

	uint16_t color;

	if(wireframe){
		color = 0b0000011111100000;
		draw_line(pixelBuffer,coordX[0],coordY[0],coordX[1],coordY[1],color);
		draw_line(pixelBuffer,coordX[1],coordY[1],coordX[2],coordY[2],color);
		draw_line(pixelBuffer,coordX[2],coordY[2],coordX[0],coordY[0],color);
	}else{

		if(shadow){
			color = (uint16_t)((BLUE/255.f) * 0b11111) * 0.2f;
			color |= (uint16_t)(((GREEN/255.f) * 0b111111) * 0.2f) << 5;
			color |= (uint16_t)(((RED/255.f) * 0b11111) * 0.2f) << 11;
		}else{
			color = (uint16_t)((BLUE/255.f) * 0b11111) * sc_multiply;
			color |= (uint16_t)(((GREEN/255.f) * 0b111111) * sc_multiply) << 5;
			color |= (uint16_t)(((RED/255.f) * 0b11111) * sc_multiply) << 11;
		}



//		color = shadow ? (uint16_t)(drawingColor * 0.2f) : (uint16_t)(drawingColor * sc_multiply);

		
    draw_filled_triangle(pixelBuffer, (float)coordX[0], (float)coordY[0], (float)coordX[1], (float)coordY[1], (float)coordX[2], (float)coordY[2], color);
	}
}

void sort_triangles(obj_t* obj, int* indices, int length){
	if(length <= 1)return;
	float pivot = 0.f;
	for(int i = 0; i < 3; i++){
		pivot += (obj->triangles)[indices[length-1]]->vertex[i][0];
	}
	int newLen = 0;
	for(int i = 0; i < length - 1; i++){
		float val = 0.f;
		for(int j = 0; j < 3; j++){
			val += (obj->triangles)[indices[i]]->vertex[j][0];
		}
		if(val < pivot)newLen++;
	}
	int temp = indices[newLen];
	indices[newLen] = indices[length-1];
	indices[length - 1] = temp;
	for(int i = 0; i < newLen; i++){
		float val = 0.f;
		for(int j = 0; j < 3; j++){
			val += (obj->triangles)[indices[i]]->vertex[j][0];
		}
		if(val >= pivot){
			for(int j = newLen+1; j < length; j++){
				float val2 = 0.f;
				for(int k = 0; k < 3; k++){
					val2 += (obj->triangles)[indices[j]]->vertex[k][0];
				}
				if(val2 < pivot){
					int temp = indices[j];
					indices[j] = indices[i];
					indices[i] = temp;
				}
			}
		}
	}

	sort_triangles(obj,indices, newLen);
	sort_triangles(obj,indices+newLen+1, length-newLen-1);
}


void proj_obj(obj_t* obj, camera_t cam, float light[3], uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
	int indices[obj->quantity];
	for(int i = 0; i < obj->quantity; i++){
		indices[i] = i;
	}
	sort_triangles(obj, indices, obj->quantity);
	for(int i = obj->quantity-1; i >= 0; i--){
		proj_triangle(*(obj->triangles)[indices[i]],cam,light,false,pixelBuffer);
	}
}
