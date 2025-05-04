#ifndef KNOB_UTILS_H
#define KNOB_UTILS_H

#include <stdbool.h>

#include "render.h"
#include "object_transformations.h"
#include "mzapo_regs.h"
#include "directory.h"

#define DOUBLE_CLICK_TOLERANCE 0.2 // Doensn't work
extern const int MODE_MAX;


typedef struct knobs_t {
    uint32_t rgb_knobs_value;
    signed char encoders_values[3];
    signed char encoders_diff[3];
    bool encoders_pressed[3];
    bool encoders_switched[3];
} knobs_t;

knobs_t* initialize_knobs();

void read_knobs_values(unsigned char* mem_base, knobs_t* knobs);

void choose_file(knobs_t* knobs, directory_t* dir);

void switch_state(int* state, knobs_t* knobs);

void check_mode(enum Mode* mode, bool choose_mode, knobs_t* knobs);

void check_rotation(obj_t* obj, camera_t* cam, knobs_t* knobs);

#endif /* KNOB_UTILS_H */
