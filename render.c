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

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
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


void draw_filled_triangle(uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) {
    // y sorting
    if (y2 < y1) { int t = y1; y1 = y2; y2 = t; t = x1; x1 = x2; x2 = t; }
    if (y3 < y1) { int t = y1; y1 = y3; y3 = t; t = x1; x1 = x3; x3 = t; }
    if (y3 < y2) { int t = y2; y2 = y3; y3 = t; t = x2; x2 = x3; x3 = t; }

    float inv_slope1, inv_slope2;
    int y;

    if (y2 - y1 != 0) {
        inv_slope1 = (float)(x2 - x1) / (y2 - y1);
        inv_slope2 = (float)(x3 - x1) / (y3 - y1);
        float curx1 = x1;
        float curx2 = x1;

        for (y = y1; y <= y2; y++) {
            int x_start = (int)curx1;
            int x_end = (int)curx2;
            if (x_start > x_end) { int t = x_start; x_start = x_end; x_end = t; }
            for (int x = x_start; x <= x_end; x++) {
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
                    pixelBuffer[y][x] = color;
            }
            curx1 += inv_slope1;
            curx2 += inv_slope2;
        }
    }

    if (y3 - y2 != 0) {
        inv_slope1 = (float)(x3 - x2) / (y3 - y2);
        inv_slope2 = (float)(x3 - x1) / (y3 - y1);
        float curx1 = x2;
        float curx2 = x1 + inv_slope2 * (y2 - y1);

        for (y = y2; y <= y3; y++) {
            int x_start = (int)curx1;
            int x_end = (int)curx2;
            if (x_start > x_end) { int t = x_start; x_start = x_end; x_end = t; }
            for (int x = x_start; x <= x_end; x++) {
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
                    pixelBuffer[y][x] = color;
            }
            curx1 += inv_slope1;
            curx2 += inv_slope2;
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

    	draw_filled_triangle(pixelBuffer, coordX[0], coordY[0], coordX[1], coordY[1], coordX[2], coordY[2], color);
	}
}

void proj_objs(obj_t* objs, int obj_quantity, camera_t cam, float light[3], uint16_t pixelBuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    int triangles_quantity = 0;
    for (int i = 0; i < obj_quantity; i++)
        triangles_quantity += objs[i].quantity;

    triangle_t** triangles = malloc(sizeof(triangle_t*) * triangles_quantity);
    float* distances = malloc(sizeof(float) * triangles_quantity);
    if (!triangles || !distances) {
        fprintf(stderr, "ERROR: can't allocate memory!\n");
        exit(ERROR_MALLOC);
    }

    int t_index = 0;
    for (int i = 0; i < obj_quantity; i++)
        for (int j = 0; j < objs[i].quantity; j++)
            triangles[t_index++] = objs[i].triangles[j];

    for (int i = 0; i < triangles_quantity; i++) {
        float center[3] = {
            (triangles[i]->vertex[0][0] + triangles[i]->vertex[1][0] + triangles[i]->vertex[2][0]) / 3.f,
            (triangles[i]->vertex[0][1] + triangles[i]->vertex[1][1] + triangles[i]->vertex[2][1]) / 3.f,
            (triangles[i]->vertex[0][2] + triangles[i]->vertex[1][2] + triangles[i]->vertex[2][2]) / 3.f
        };

        float diff[3] = { center[0] - cam.coord[0], center[1] - cam.coord[1], center[2] - cam.coord[2] };
        distances[i] = scalar_multiply(diff, diff);
    }

    // Painter's algorithm
    for (int i = 0; i < triangles_quantity - 1; i++) {
        for (int j = i + 1; j < triangles_quantity; j++) {
            if (distances[i] < distances[j]) {
                float tmp_dist = distances[i];
                distances[i] = distances[j];
                distances[j] = tmp_dist;

                triangle_t* tmp_t = triangles[i];
                triangles[i] = triangles[j];
                triangles[j] = tmp_t;
            }
        }
    }

    for (int i = 0; i < triangles_quantity; i++) {
        float center[3] = {
            (triangles[i]->vertex[0][0] + triangles[i]->vertex[1][0] + triangles[i]->vertex[2][0]) / 3.f,
            (triangles[i]->vertex[0][1] + triangles[i]->vertex[1][1] + triangles[i]->vertex[2][1]) / 3.f,
            (triangles[i]->vertex[0][2] + triangles[i]->vertex[1][2] + triangles[i]->vertex[2][2]) / 3.f
        };

        float direction[3] = { center[0] - light[0], center[1] - light[1], center[2] - light[2] };
        float len = sqrt(scalar_multiply(direction, direction));
        float norm_dir[3] = { direction[0] / len, direction[1] / len, direction[2] / len };
        float origin[3] = {
            light[0] + norm_dir[0] * 0.001f,
            light[1] + norm_dir[1] * 0.001f,
            light[2] + norm_dir[2] * 0.001f
        };

        bool shadow = false;
        for (int j = 0; j < triangles_quantity; j++) {
            if (i == j) continue;
            float t;
            if (vec_triangle_collided(*triangles[j], origin, norm_dir, &t) && t < len) {
                shadow = true;
                break;
            }
        }

        proj_triangle(*triangles[i], cam, light, shadow, pixelBuffer);
    }

    free(triangles);
    free(distances);
}
