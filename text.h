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
 
// when using makefile
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern font_descriptor_t *fdes;
extern int scale;
 
unsigned int hsv2rgb_lcd(int hue, int saturation, int value);
 
void draw_pixel_1(int x, int y, unsigned short color);
void draw_pixel_text(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, unsigned short color);

void draw_pixel_1_big(int x, int y, unsigned short color);
void draw_pixel_text_big(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, unsigned short color);
 
int char_width(int ch);

void draw_char(int x, int y, char ch, unsigned short color);
void draw_char_test(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, char ch, unsigned short color);

void draw_string(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, char* str, unsigned short color);

#endif /* TEXT_H */