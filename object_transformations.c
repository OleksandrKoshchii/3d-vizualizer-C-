#include <math.h>



#include "object_transformations.h"
#include "matrix_operations.h"
#include "read_stl.h"


#define VERTICES_QUANTITY 3
#define DIMENSION 3

void rotate_obj_horizontal(obj_t* obj,float angle){

	obj->rotationZ -= angle;	//add minus, because we have got here counterclockwise rotation
	angle = ang_to_rad(angle);

	float matrix_rotation[DIMENSION][DIMENSION] = {
		{cos(angle),sin(angle),0},
		{-sin(angle),cos(angle),0},
		{0,0,1},
	};

	for(int triangle = 0; triangle < obj->quantity; triangle++){

		multiply_mat_vec(obj->triangles[triangle]->normal,matrix_rotation);

		for(int vertex = 0; vertex < VERTICES_QUANTITY; vertex++){
			
			float transformed_vec[3];

			for(int coord = 0; coord < DIMENSION; coord++){
				obj->triangles[triangle]->vertex[vertex][coord] -= obj->pivot[coord];
			};

			multiply_mat_vec(obj->triangles[triangle]->vertex[vertex],matrix_rotation);

			for(int coord = 0; coord < DIMENSION; coord++){
				obj->triangles[triangle]->vertex[vertex][coord] += obj->pivot[coord];
			};
		};
	}
}


void rotate_obj_vertical(obj_t* obj,float angle){
	
	obj->rotationX += angle;
	angle = ang_to_rad(angle);	

	float matrix_rotation[DIMENSION][DIMENSION];
	if(matrix_rotation){
		float rotateZ = ang_to_rad(obj->rotationZ);


		float matrix_rotate_backZ[DIMENSION][DIMENSION] = {
			{cos(-rotateZ),sin(-rotateZ),0},
			{-sin(-rotateZ),cos(-rotateZ),0},
			{0,0,1},
		};

		float matrix_rotateX[DIMENSION][DIMENSION] = {
			{1,0,0},
			{0,cos(angle),sin(angle)},
			{0,-sin(angle),cos(angle)},
		};

		float temp[DIMENSION][DIMENSION];
		multiply_mat_mat(matrix_rotateX,matrix_rotate_backZ,temp);

		float matrix_rotateZ[DIMENSION][DIMENSION] = {
			{cos(rotateZ),sin(rotateZ),0},
			{-sin(rotateZ),cos(rotateZ),0},
			{0,0,1},
		};
		
		multiply_mat_mat(matrix_rotateZ,temp,matrix_rotation);
	}

	for(int triangle = 0; triangle < obj->quantity; triangle++){

		multiply_mat_vec(obj->triangles[triangle]->normal,matrix_rotation);

		for(int vertex = 0; vertex < VERTICES_QUANTITY; vertex++){
			
			float transformed_vec[3];

			for(int coord = 0; coord < DIMENSION; coord++){
				obj->triangles[triangle]->vertex[vertex][coord] -= obj->pivot[coord];
			};

			multiply_mat_vec(obj->triangles[triangle]->vertex[vertex],matrix_rotation);

			for(int coord = 0; coord < DIMENSION; coord++){
				obj->triangles[triangle]->vertex[vertex][coord] += obj->pivot[coord];
			};
		};
	}
}
