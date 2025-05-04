#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "read_stl.h"
#include "render.h"
#include "directory.h"
#include "knob.h"

bool waiting_for_second_click = false;
clock_t first_click_time = 0;
int knob_threshold = 3;

knobs_t* initialize_knobs() {
    knobs_t* knobs;
    knobs = malloc(sizeof(knobs_t));
    if(knobs == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    for(int i = 0; i < 3; i++) {
        knobs->encoders_values[i] = 0;
        knobs->encoders_diff[i] = 0;
        knobs->encoders_pressed[i] = false;
        knobs->encoders_switched[i] = false;
    }
    knobs->rgb_knobs_value = 0;

    return knobs;
}

void read_knobs_values(unsigned char* mem_base, knobs_t* knobs) {
	knobs->rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
	
    for(int encoder = 0; encoder < 3; encoder++) {
		
		signed char lastValue = knobs->encoders_values[encoder];
		bool lastStatus = knobs->encoders_pressed[encoder];

		knobs->encoders_values[encoder] = (knobs->rgb_knobs_value >> 8 * encoder) & 0xFF;
		// Original
		// knobs->encodersPressed[encoder] = ((((knobs->rgb_knobs_value >> 24) >> encoder) & 0x01) == 0x01) ? true : false;
        // Alternative	
        knobs->encoders_pressed[encoder] = ((knobs->rgb_knobs_value >> (24 + encoder)) & 0x01) == 0x01;
		
		knobs->encoders_switched[encoder] = (lastStatus == false && knobs->encoders_pressed[encoder] == true);

		knobs->encoders_diff[encoder] = knobs->encoders_values[encoder] - lastValue;
	}
}

void choose_file(knobs_t* knobs, directory_t* dir) {
	if (knobs->encoders_diff[1] > knob_threshold) {
		dir->active_file++;
		if (dir->active_file >= dir->file_count) {
			dir->active_file = 0;
		}
	}
	if(knobs->encoders_diff[1] < -(knob_threshold)) {
		dir->active_file--;
		if(dir->active_file < 0) {
			dir->active_file = dir->file_count - 1;
		}
	}
}

void switch_state(int* state, knobs_t* knobs) {
	if(knobs->encoders_switched[1]) {
		first_click_time = clock();
		if(!waiting_for_second_click) {
			waiting_for_second_click = true;
		}
		// else {
		// 	if(knobs->encoders_switched[1]) {
        //         double elapsed = ((clock() - first_click_time) / CLOCKS_PER_SEC);  
        //         // DEBUG
        //         printf("Elapsed: %f\n", elapsed);
		// 		if(elapsed < DOUBLE_CLICK_TOLERANCE) {
		// 			*state = (*state == 0) ? 1 : 0;
		// 		}
		// 		waiting_for_second_click = false;
		// 	}
		// }
		else {
            // Second click detected
            double elapsed = ((clock() - first_click_time) / CLOCKS_PER_SEC);
            printf("Elapsed: %f\n", elapsed);

            if(elapsed < DOUBLE_CLICK_TOLERANCE) {
                *state = (*state == 0) ? 1 : 0;
            }
            waiting_for_second_click = false;
        }
	}
	//Reset if too late
	if(waiting_for_second_click && ((clock() - first_click_time) / CLOCKS_PER_SEC > DOUBLE_CLICK_TOLERANCE)) {
		waiting_for_second_click = false;
	}
}

void check_mode(enum Mode* mode, bool choose_mode, knobs_t* knobs) {
    if(choose_mode) {
        if (knobs->encoders_diff[0] > 0) {
            (*mode)++;
            if(*mode > MODE_MAX) (*mode) = 0;
        } else if(knobs->encoders_diff[0] < 0) {
            (*mode)--;
            if((*mode) < 0) (*mode) = MODE_MAX;
        }
    }
}

void check_rotation(obj_t* obj, camera_t* cam, knobs_t* knobs) {
	if(knobs->encoders_diff[0] != 0) {
		cam->coord[0] += knobs->encoders_diff[0] * 0.05f;
	}
	if(knobs->encoders_diff[1] != 0) {
		rotate_obj_horizontal(obj, knobs->encoders_diff[1]);
	}
	if(knobs->encoders_diff[2] != 0) {
		rotate_obj_vertical(obj, knobs->encoders_diff[2]);
	}
}
