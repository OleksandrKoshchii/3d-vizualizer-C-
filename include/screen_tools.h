#ifndef __SCREEN_TOOLS__
#define __SCREEN_TOOLS__

#include <stdbool.h>

bool init_screen(char* name);

void destroy_screen();

float from_x_coord(int old_number);

float from_y_coord(int old_number);

int to_x_coord(float old_number);

int to_y_coord(float old_number);
#endif
