#include <stdio.h>
#include <stdlib.h>

#include "read_stl.h"

#define DIMENSION 3
#define VERTICES_QUANTITY 3

obj_t readBinarySTL(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("ERROR: Unable to open file\n");
        exit(-2);
    }

    char header[80];
    fread(header, sizeof(char), 80, file); // Чтение заголовка

    unsigned int numTriangles;
    fread(&numTriangles, sizeof(unsigned int), 1, file); // Чтение количества треугольников

	triangle_t** triangles = (triangle_t**)malloc(numTriangles * sizeof(triangle_t*));

	if(!triangles){
		fprintf(stderr,"ERROR: malloc!\n");
		exit(-1);
	}

    for (unsigned int i = 0; i < numTriangles; i++) {
        triangle_t* triangle = (triangle_t*)malloc(sizeof(triangle_t));
		if(!triangle){
			fprintf(stderr,"ERROR: malloc!\n");
			exit(-1);
		}


        fread(&triangle->normal, sizeof(float), 3, file);
        fread(&triangle->vertex[0], sizeof(float), 3, file);
        fread(&triangle->vertex[1], sizeof(float), 3, file);
        fread(&triangle->vertex[2], sizeof(float), 3, file);
        fread(&triangle->attributeByteCount, sizeof(unsigned short), 1, file);

		triangles[i] = triangle;
    }

    fclose(file);
	
	obj_t result;
	result.rotationX = 0.f;
	result.rotationY = 0.f;
	result.rotationZ = 0.f;
	result.triangles = triangles;
	result.quantity = numTriangles;
	
	for(int i = 0; i < DIMENSION; i++){
		float average = 0;
		for(int j = 0; j < result.quantity; j++){
			for(int k = 0; k < VERTICES_QUANTITY; k++){
				average += result.triangles[j]->vertex[k][i];
			}
		}
		average /= result.quantity * VERTICES_QUANTITY;
		result.pivot[i] = average;
	}
	

	return result;
}

void free_obj(obj_t* obj){
	for(int i = 0; i < obj->quantity; i++){
		free(obj->triangles[i]);
	}
	free(obj->triangles);
}
