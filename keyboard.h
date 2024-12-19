#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdio.h>
#include "pico/stdlib.h"

// Define constants for rows, columns, and their corresponding pins
#define ROWS 4
#define COLS 4

// Function prototypes
void gpio_init_keypad(void);
char scanKeypad(void);

#endif