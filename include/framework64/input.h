#ifndef FW64_INPUT_H
#define FW64_INPUT_H

#include "vec2.h"

#include <nusys.h>

typedef struct {
    NUContData 	current_state[NU_CONT_MAXCONTROLLERS];
    NUContData 	previous_state[NU_CONT_MAXCONTROLLERS];
} Input;

void input_init(Input* input);
void input_update(Input* input);

int input_button_pressed(Input* input, int controller, int button);
int input_button_down(Input* input, int controller, int button);
void input_stick(Input* input, int controller, Vec2* current);

#endif