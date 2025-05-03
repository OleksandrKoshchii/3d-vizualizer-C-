#ifndef KNOB_UTILS_H
#define KNOB_UTILS_H

#include <stdbool.h>

#include "mzapo_regs.h"

#define DOUBLE_CLICK_TOLERANCE 0.2 // Doensn't work

typedef struct knobs_t {
    uint32_t rgb_knobs_value;
    signed char encodersValues[3];
    signed char encodersDif[3];
    bool encodersPressed[3];
    bool encodersSwitched[3];
} knobs_t;

knobs_t* initialize_knobs();

void read_knobs_values(unsigned char* mem_base, knobs_t* knobs);

void choose_file(knobs_t* knobs, directory_t* dir);

void check_state(int* state, knobs_t* knobs);

#endif /* KNOB_UTILS_H */
