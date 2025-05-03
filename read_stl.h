#ifndef __READ_BINARY_STL__
#define __READ_BINARY_STL__

typedef struct {
	float normal[3];
	float vertex[3][3];
	unsigned short attributeByteCount;
} triangle_t;

typedef struct {
	triangle_t** triangles;
	int quantity;
	float pivot[3];
	float rotationX;
	float rotationY;
	float rotationZ;
} obj_t;

obj_t readBinarySTL(const char* filename);

void free_obj(obj_t* obj);

#endif
