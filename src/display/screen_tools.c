#include <stdbool.h>

#include "render.h"


float from_x_coord(int old_number){

	float old_min = 0;
	float new_min = -1;
	float new_range = 2;
	float old_range = SCREEN_WIDTH;
	

	float new_number =((((float)old_number-old_min) * new_range) / old_range)+new_min;

	return new_number;
}

float from_y_coord(int old_number){

	float old_min = 0;
	float new_min = -1;
	float new_range = 2;
	float old_range = SCREEN_HEIGHT;
	

	float new_number =((((float)old_number-old_min) * new_range) / old_range)+new_min;

	return new_number;
}

int to_x_coord(float old_number){

	float old_min = -1;
	float new_min = 0;
	float new_range = SCREEN_WIDTH;
	float old_range = 2;
	

	float new_number =((((float)old_number-old_min) * new_range) / old_range)+new_min;

	return (int)new_number;
}

int to_y_coord(float old_number){

	float old_min = -1;
	float new_min = 0;
	float new_range = SCREEN_HEIGHT;
	float old_range = 2;
	

	float new_number =((((float)old_number-old_min) * new_range) / old_range)+new_min;

	return (int)new_number;
}


