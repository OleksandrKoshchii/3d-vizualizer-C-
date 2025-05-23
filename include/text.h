#ifndef TEXT_H
#define TEXT_H
 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
 
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "font_types.h"
#include "render.h"
 
extern font_descriptor_t *fdes;
 
unsigned int hsv2rgb_lcd(int hue, int saturation, int value);
 
void draw_pixel_text(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, unsigned short color);

void draw_pixel_text_big(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, unsigned short color, int scale);
 
int char_width(int ch);

int string_width(char* string);

void draw_char_text(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, char ch, unsigned short color, int scale);

void draw_string(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, char* str, unsigned short color, int scale);

#endif /* TEXT_H */
