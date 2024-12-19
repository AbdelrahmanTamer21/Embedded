#ifndef LCD_H
#define LCD_H

#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#define I2C_PORT i2c1
#define MAX_LINES 2
#define MAX_CHARS 16
#define LCD_SDA_PIN 2
#define LCD_SCL_PIN 3

void LCD_CMD(uint8_t command);
void lcd_set_cursor(int line, int position);
void LCD_DATA(uint8_t data);
void lcd_print_string(const char *message);
void lcd_init();
void lcd_clear();
#endif