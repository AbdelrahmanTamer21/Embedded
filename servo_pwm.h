#ifndef SERVO_PWM_H
#define SERVO_PWM_H

#include "pico/stdlib.h"

// Function to initialize the servo motor on a given GPIO pin
void servo_init(uint gpio_pin);

// Function to set the servo angle (0° to 180°) on a given GPIO pin
void servo_set_angle(uint gpio_pin, float angle);

#endif // SERVO_PWM_H
