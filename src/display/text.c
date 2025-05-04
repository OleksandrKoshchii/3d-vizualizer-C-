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

#include "text.h"

int scale = 2;

unsigned int hsv2rgb_lcd(int hue, int saturation, int value) {
    hue = (hue%360);    
    float f = ((hue%60)/60.0);
    int p = (value*(255-saturation))/255;
    int q = (value*(255-(saturation*f)))/255;
    int t = (value*(255-(saturation*(1.0-f))))/255;
    unsigned int r,g,b;
    if (hue < 60){
        r = value; g = t; b = p;
    } else if (hue < 120) {
        r = q; g = value; b = p;
    } else if (hue < 180) {
        r = p; g = value; b = t;
    } else if (hue < 240) {
        r = p; g = q; b = value;
    } else if (hue < 300) {
        r = t; g = p; b = value;
    } else {
        r = value; g = p; b = q;
    }
    r>>=3;
    g>>=2;
    b>>=3;
    return (((r&0x1f)<<11)|((g&0x3f)<<5)|(b&0x1f));
}

void draw_pixel_text(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, unsigned short color) {
    if (x>=0 && x<SCREEN_WIDTH && y>=0 && y<SCREEN_HEIGHT) {
        buffer[y][x] = color;  // [y][x] to match main.c
    }
}

void draw_pixel_text_big(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, unsigned short color) {
    int i,j;
    for (i=0; i<scale; i++) {
        for (j=0; j<scale; j++) {
            draw_pixel_text(buffer, x+i, y+j, color);
        }
    }
}

int char_width(int ch) {
    
    if (!fdes) {
        fprintf(stderr, "Font not initialized!\n");
        return 0;
    }

    int width;

    if (!fdes->width) {
        width = fdes->maxwidth;
    } else {
        width = fdes->width[ch-fdes->firstchar];
    }
    return width;
}

int string_width(char* string) {
    int width = 0;
    for(int i = 0; string[i] != '\0'; i++) {
        width += char_width(string[i]);
    }
    return width;
}

void draw_char_text(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, char ch, unsigned short color) {
    if (!fdes) {
        fprintf(stderr, "Font not initialized!\n");
        return;
    }

    int w = char_width(ch);
    const font_bits_t *ptr;
    if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
        if (fdes->offset) {
            ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
        } else {
            int bw = (fdes->maxwidth+15)/16;
            ptr = &fdes->bits[(ch-fdes->firstchar)*bw*fdes->height];
        }
        int i, j;
        for (i=0; i<fdes->height; i++) {
            font_bits_t val = *ptr;
            for (j=0; j<w; j++) {
                if ((val&0x8000)!=0) {
                    draw_pixel_text_big(buffer, x+scale*j, y+scale*i, color);
                }
                val<<=1;
            }
            ptr++;
        }
    }
}

void draw_string(uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH], int x, int y, char* string, unsigned short color) {
    int pos_x = x;
    for (int i = 0; string[i] != '\0'; i++) {
        draw_char_text(buffer, pos_x, y, string[i], color);
        pos_x += char_width(string[i]) * scale;
    }
}