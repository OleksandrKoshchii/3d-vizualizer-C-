#ifndef __MATRIX_OPERATIONS__
#define __MATRIX_OPERATIONS__
#define SIZE 3

#include <stdbool.h>

#include "render.h"

#define DIMENSION 3

float determinant(float matrix[SIZE][SIZE],int n);

void adjugate(float matrix[SIZE][SIZE], float adj[SIZE][SIZE]);

bool inverse(float matrix[SIZE][SIZE],float inverse[SIZE][SIZE]);

void multiply_mat_vec(float vec[3],float matrix[3][3]);

void multiply_mat_mat(float matrix1[DIMENSION][DIMENSION],float matrix2[DIMENSION][DIMENSION],float result[DIMENSION][DIMENSION]);

float ang_to_rad(float old);

float rad_to_ang(float old);

void rotate_cam_horizontal(camera_t* cam,float angle);

void rotate_cam_vertical(camera_t* cam,float angle);

float scalar_multiply(float vec1[DIMENSION],float vec2[DIMENSION]);

void vec_multiply(float vec1[DIMENSION],float vec2[DIMENSION],float result[DIMENSION]);//dim must be 3

void add_vectors(float vec1[DIMENSION],float vec2[DIMENSION],float result[DIMENSION]);

void diff_vectors(float vec1[DIMENSION],float vec2[DIMENSION],float result[DIMENSION]);
#endif

