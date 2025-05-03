#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "directory.h"
#include "knob.h"

bool waiting_for_second_click = false;
clock_t first_click_time = 0;

#define DOUBLE_CLICK_TOLERANCE 0.2 // Doensn't work
int knob_threshold = 3;

knobs_t* initialize_knobs() {
    knobs_t* knobs;
    knobs = malloc(sizeof(knobs_t));
    if(knobs == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    for(int i = 0; i < 3; i++) {
        knobs->encodersValues[i] = 0;
        knobs->encodersDif[i] = 0;
        knobs->encodersPressed[i] = false;
        knobs->encodersSwitched[i] = false;
    }
    knobs->rgb_knobs_value = 0;

    return knobs;
}

void read_knobs_values(unsigned char* mem_base, knobs_t* knobs) {
	knobs->rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
	
    for(int encoder = 0; encoder < 3; encoder++) {
		
		signed char lastValue = knobs->encodersValues[encoder];
		bool lastStatus = knobs->encodersPressed[encoder];

		knobs->encodersValues[encoder] = (knobs->rgb_knobs_value >> 8 * encoder) & 0xFF;
		// Original
		// knobs->encodersPressed[encoder] = ((((knobs->rgb_knobs_value >> 24) >> encoder) & 0x01) == 0x01) ? true : false;
        // Alternative	
        knobs->encodersPressed[encoder] = ((knobs->rgb_knobs_value >> (24 + encoder)) & 0x01) == 0x01;
		
		knobs->encodersSwitched[encoder] = (lastStatus == false && knobs->encodersPressed[encoder] == true);

		knobs->encodersDif[encoder] = knobs->encodersValues[encoder] - lastValue;
	}
}

void choose_file(knobs_t* knobs, directory_t* dir) {
	if (knobs->encodersDif[1] > knob_threshold) {
		dir->active_file++;
		if (dir->active_file >= dir->file_count) {
			dir->active_file = 0;
		}
	}
	if(knobs->encodersDif[1] < -knob_threshold) {
		dir->active_file--;
		if(dir->active_file < 0) {
			dir->active_file = dir->file_count - 1;
		}
	}
}

void check_state(int* state, knobs_t* knobs) {
	if(knobs->encodersSwitched[1]) {
		first_click_time = clock();
		if(!waiting_for_second_click) {
			waiting_for_second_click = true;
		}
		else {
			if(knobs->encodersSwitched[1]) {
                double elapsed = ((clock() - first_click_time) / CLOCKS_PER_SEC);
                
                // DEBUG
                printf("Elapsed: %f\n", elapsed);

				if(elapsed < DOUBLE_CLICK_TOLERANCE) {
					*state = (*state == 0) ? 1 : 0;
				}
				waiting_for_second_click = false;
			}
		}
	}
	//Reset if too late
	if(waiting_for_second_click && ((clock() - first_click_time) / CLOCKS_PER_SEC > DOUBLE_CLICK_TOLERANCE)) {
		waiting_for_second_click = false;
	}
}
