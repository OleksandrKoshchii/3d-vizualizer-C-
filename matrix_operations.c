#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "matrix_operations.h"
#include "render.h"

#define DIMENSION 3

float determinant(float matrix[SIZE][SIZE], int n) {
    float det = 0;
    if (n == 1) {
        return matrix[0][0];
    }
    float submatrix[SIZE][SIZE];
    for (int x = 0; x < n; x++) {
        int subi = 0;
        for (int i = 1; i < n; i++) {
            int subj = 0;
            for (int j = 0; j < n; j++) {
                if (j == x) continue;
                submatrix[subi][subj] = matrix[i][j];
                subj++;
            }
            subi++;
        }
        det += (x % 2 == 0 ? 1 : -1) * matrix[0][x] * determinant(submatrix, n - 1);
    }
    return det;
}

void adjugate(float matrix[SIZE][SIZE], float adj[SIZE][SIZE]) {
    float submatrix[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int subi = 0;
            for (int row = 0; row < SIZE; row++) {
                if (row == i) continue;
                int subj = 0;
                for (int col = 0; col < SIZE; col++) {
                    if (col == j) continue;
                    submatrix[subi][subj] = matrix[row][col];
                    subj++;
                }
                subi++;
            }
            adj[j][i] = ((i + j) % 2 == 0 ? 1 : -1) * determinant(submatrix, SIZE - 1);
        }
    }
}

bool inverse(float matrix[SIZE][SIZE], float inverse[SIZE][SIZE]) {
    float det = determinant(matrix, SIZE);
    if (det == 0) {
        printf("Обратная матрица не существует (определитель равен нулю).\n");
        return false;
    }

    float adj[SIZE][SIZE];
    adjugate(matrix, adj);
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            inverse[i][j] = adj[i][j] / det;
        }
    }
    return true;
}

void multiply_mat_vec(float vec[3],float matrix[3][3]){

	float new_vec[3];

	for(int i = 0; i < 3; i++){
		new_vec[i] = 0;

		for(int j = 0; j < 3; j++){
			new_vec[i]+=vec[j]*matrix[j][i];
		}
	}

	for(int i = 0; i < 3; i++){
		vec[i] = new_vec[i];
	}
}

void multiply_mat_mat(float matrix1[DIMENSION][DIMENSION],float matrix2[DIMENSION][DIMENSION], float result[DIMENSION][DIMENSION]){
	for (int i = 0; i < DIMENSION; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            result[i][j] = 0.0f; 
            for (int k = 0; k < DIMENSION; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

float ang_to_rad(float old){
	return (old/180.f) * M_PI;
}

float rad_to_ang(float old){
	return (old/M_PI)*180.f;
}

void rotate_cam_horizontal(camera_t* cam,float angle){

	angle = ang_to_rad(angle);

	float matrix_rotation[3][3] = {
	/*	{cos(ang_to_rad(angle)),sin(ang_to_rad(angle)),0},
		{-sin(ang_to_rad(angle)),cos(ang_to_rad(angle)),0},
		{0,0,1},*/
	/*	{cos(ang_to_rad(angle)),0,sin(ang_to_rad(angle))},
		{0,1,0},
		{-sin(ang_to_rad(angle)),0,cos(ang_to_rad(angle))},*/
		{cos(angle),sin(angle),0},
		{-sin(angle),cos(angle),0},
		{0,0,1},
	};


	for(int i = 0; i < 3; i++){
		float vec[3];
		for(int j = 0; j < 3; j++){
			vec[j] = cam->orientation[i][j];
		}
		multiply_mat_vec(vec,matrix_rotation);
		for(int j = 0; j < 3; j++){
			cam->orientation[i][j] = vec[j];
		}
	}	
/*

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			cam->orientation[i][j] = matrix_rotation[i][j];
		}
	}*/
}

void rotate_cam_vertical(camera_t* cam,float angle){

	angle = ang_to_rad(angle);

	float matrix_rotation[3][3] = {
	/*	{cos(ang_to_rad(angle)),sin(ang_to_rad(angle)),0},
		{-sin(ang_to_rad(angle)),cos(ang_to_rad(angle)),0},
		{0,0,1},*/
		{1,0,0},
		{0,cos(angle),sin(angle)},
		{0,-sin(angle),cos(angle)},
	};

	for(int i = 0; i < 3; i++){
		multiply_mat_vec(matrix_rotation[i],cam->orientation);
	}	

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			cam->orientation[i][j] = matrix_rotation[i][j];
		}
	}
}

float scalar_multiply(float vec1[3],float vec2[3]){
	float result = 0;
	for(int i = 0; i < 3; i++){
		result += vec1[i]*vec2[i];
	}
	return result;
}

void vec_multiply(float vec1[DIMENSION],float vec2[DIMENSION],float result[DIMENSION]){
	result[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
	result[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
	result[2] = vec1[0]*vec2[1] - vec1[1]*vec2[0];
}

void add_vectors(float vec1[DIMENSION],float vec2[DIMENSION],float result[DIMENSION]){
	for(int cor = 0; cor < DIMENSION; cor++){
		result[cor] = vec1[cor] + vec2[cor];
	}
}

void diff_vectors(float vec1[DIMENSION],float vec2[DIMENSION],float result[DIMENSION]){
	for(int cor = 0; cor < DIMENSION; cor++){
		result[cor] = vec1[cor] - vec2[cor];
	}
}
