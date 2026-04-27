#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

void buttons_init(void);

bool button_up_pressed(void);
bool button_down_pressed(void);
bool button_left_pressed(void);
bool button_right_pressed(void);

#endif