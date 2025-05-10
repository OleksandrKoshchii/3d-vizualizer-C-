#ifndef KNOB_UTILS_H
#define KNOB_UTILS_H

#include <stdbool.h>

#include "render.h"
#include "object_transformations.h"
#include "mzapo_regs.h"
#include "directory.h"

#define DOUBLECLICK_TOLERANCE 0.2 // Doensn't work
#define NUM_KNOBS 3
extern const int MODE_MAX;

typedef struct knobs_t {
    uint32_t rgb_knobs_value;
    signed char encoders_values[NUM_KNOBS];
    signed char encoders_diff[NUM_KNOBS];
    bool encoders_pressed[NUM_KNOBS];
    bool encoders_switched[NUM_KNOBS];
    bool waiting_for_doubleclick[NUM_KNOBS];
} knobs_t;

knobs_t* initialize_knobs();

void read_knobs_values(unsigned char* mem_base, knobs_t* knobs);

bool detect_doubleclick(knobs_t* knobs, int knob_index);

bool check_shutdown(bool* running, knobs_t* knobs);

void choose_file(knobs_t* knobs, directory_t* dir);

void switch_state(int* state, knobs_t* knobs);

void check_rotation(obj_t* obj, camera_t* cam, knobs_t* knobs);


#endif /* KNOB_UTILS_H */
