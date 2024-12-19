#ifndef ALARM_H
#define ALARM_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define ALARM_PIN 11

void alarm_init();
void alarm_on();
void alarm_off();

#endif // ALARM_H