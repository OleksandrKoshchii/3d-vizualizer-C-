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

//.....drawing properties.....//
extern bool wireframe;
extern uint8_t RED;
extern uint8_t GREEN;
extern uint8_t BLUE;
//.....drawing properties.....//

//.....MODES.....//
extern bool changing_color;
extern bool changing_light_pos;
//.....MODES.....//


clock_t first_click_time = 0;
int knob_threshold = 2;

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
		knobs->waiting_for_doubleclick[i] = false;
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

bool detect_doubleclick(knobs_t* knobs, int knob_index) {
	if(knob_index >= NUM_KNOBS) {
		fprintf(stderr, "ERROR: knob_index: %d is out of range!\n", knob_index);
		return false;
	}

	if(knobs->encoders_switched[knob_index]) {
		first_click_time = clock();
		if(!knobs->waiting_for_doubleclick[knob_index]) {
			knobs->waiting_for_doubleclick[knob_index] = true;
		}
		else {
            // Second click detected
            double elapsed = ((clock() - first_click_time) / CLOCKS_PER_SEC);
            // printf("Elapsed: %f\n", elapsed);

            if(elapsed < DOUBLECLICK_TOLERANCE) {
                return true;
            }
            knobs->waiting_for_doubleclick[knob_index] = false;
        }
	}

	//Reset if too late
	if(knobs->waiting_for_doubleclick[knob_index] && ((clock() - first_click_time) / CLOCKS_PER_SEC > DOUBLECLICK_TOLERANCE)) {
		knobs->waiting_for_doubleclick[knob_index] = false;
	}

	return false;
}

bool check_shutdown(bool* running, knobs_t* knobs) {
	if(knobs->encoders_switched[2]) {
		*running = false;
		return true;
	}
	return false;
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
	if(detect_doubleclick(knobs, 1)) {
		*state = (*state == 0) ? 1 : 0;
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

void switch_mode(knobs_t* knobs){
	if(knobs->encoders_switched[0] != 0){
		wireframe = !wireframe;
	}else if(knobs->encoders_switched[1] != 0){
		changing_color = !changing_color;
	}else if(knobs->encoders_switched[2] != 0){
		changing_light_pos = !changing_light_pos;
	}
}

void change_color(knobs_t* knobs){
	if(knobs->encoders_diff[0] != 0){
		BLUE += knobs->encoders_diff[0];
	}
	if(knobs->encoders_diff[1] != 0){
		GREEN += knobs->encoders_diff[1];
	}
	if(knobs->encoders_diff[2] != 0){
		RED += knobs->encoders_diff[2];
	}
}

void change_light_position(knobs_t* knobs, float light[3]){
	for(int i = 0; i < 3; i++){
		if(knobs->encoders_diff[i] != 0){
			light[i] += knobs->encoders_diff[i];
		}
	}
}
