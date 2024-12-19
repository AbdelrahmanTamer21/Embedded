#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "LCD.h"

#define MAX_LINES 2
#define MAX_CHARS 16
static int addr = 0x27;

void LCD_CMD(uint8_t command)
{
    uint8_t Hc = command & (0xF0);
    Hc = Hc & ~(1 << 0) | (1 << 3);
    uint8_t Hce = Hc | (1 << 2);
    i2c_write_blocking(I2C_PORT, addr, &Hce, 1, true);
    sleep_ms(10);
    i2c_write_blocking(I2C_PORT, addr, &Hc, 1, true);
    sleep_ms(10);
    printf("I2C write H CMD failed\n");

    uint8_t Lc = (command << 4) & (0xF0);
    Lc = Lc & ~(1 << 0) | (1 << 3);
    uint8_t Lce = Lc | (1 << 2);
    i2c_write_blocking(I2C_PORT, addr, &Lce, 1, true);
    sleep_ms(10);
    i2c_write_blocking(I2C_PORT, addr, &Lc, 1, true);
    sleep_ms(10);
    printf("I2C write L CMD failed\n");
}

void lcd_set_cursor(int line, int position)
{
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    LCD_CMD(val);
}

void LCD_DATA(uint8_t data)
{
    uint8_t Hd = data & (0xF0);
    Hd = Hd | 1 | (1 << 3);
    uint8_t Hde = Hd | (1 << 2);
    i2c_write_blocking(I2C_PORT, addr, &Hde, 1, true);
    sleep_ms(1);
    i2c_write_blocking(I2C_PORT, addr, &Hd, 1, true);
    sleep_ms(1);
    printf("I2C write H DATA failed\n");
    uint8_t Ld = (data << 4) & (0xF0);
    Ld = Ld | 1 | (1 << 3);
    uint8_t Lde = Ld | (1 << 2);
    i2c_write_blocking(I2C_PORT, addr, &Lde, 1, true);
    sleep_ms(1);
    i2c_write_blocking(I2C_PORT, addr, &Ld, 1, true);
    sleep_ms(1);
    printf("I2C write L DATA failed\n");
}

void lcd_print_string(const char *message)
{
    while (*message)
    {
        LCD_DATA(*message++);
        sleep_ms(1);
        LCD_CMD(0x06);
    }
}

void lcd_clear(){
    LCD_CMD(0x01);
    sleep_ms(10);
}

void lcd_init()
{
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(LCD_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(LCD_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(LCD_SDA_PIN);
    gpio_pull_up(LCD_SCL_PIN);
    // bi_decl(bi_2pins_with_func(LCD_SDA_PIN, LCD_SCL_PIN, GPIO_FUNC_I2C));

    sleep_ms(100);

    LCD_CMD(0x03);
    LCD_CMD(0x03);
    LCD_CMD(0x03);
    LCD_CMD(0x02);
    sleep_ms(10);

    LCD_CMD(0x06);
    sleep_ms(10);

    LCD_CMD(0x28);
    sleep_ms(10);

    LCD_CMD(0x0C);
    sleep_ms(10);

    LCD_CMD(0x01);
    sleep_ms(10);
}