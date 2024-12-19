#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define TRIGGER_PIN 13
#define ECHO_PIN 14

void ultrasonic_init();

float measure_distance();

#endif