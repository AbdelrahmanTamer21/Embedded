#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/multicore.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LCD.h"
#include "servo_pwm.h"
#include "keyboard.h"
#include "ultrasonic.h"
#include "alarm.h"
#include <boards/pico.h>

#define SERVO_PIN 15 // Define the GPIO pin connected to the servo
#define RFID_PIN 6
#define RFID_ACTIVE_PIN 7

/* Task function prototypes */
void vEntryTask(void *pvParameters);
void vDistanceTask(void *pvParameters);
void vDetectionTask(void *pvParameters);

char passcode[5] = "";
int passcode_length = 0;
int start_position;

volatile bool accessGranted = false;
uint8_t uid[7];
uint8_t uid_len;

volatile bool readCard = false;

void cardDetection()
{
    if (!accessGranted && gpio_get(RFID_ACTIVE_PIN))
    {
        readCard = true;
        if (gpio_get(RFID_PIN) == true)
        {
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print_string("Access Granted");
            accessGranted = true;
        }
        else
        {
            passcode_length = 5;
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_print_string("Access Denied");
            alarm_on();
            sleep_ms(1000);
            alarm_off();
            accessGranted = false;
            passcode_length = 0;
        }
        readCard = false;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void init()
{
    stdio_init_all();   // Initialize standard I/O (optional for debugging)
    lcd_init();         // Initialize I2C communication for the LCD
    gpio_init_keypad(); // Initialize GPIO pins for the keypad
    ultrasonic_init();  // Initialize GPIO pins for the ultrasonic sensor
    gpio_init(RFID_PIN);
    gpio_set_dir(RFID_PIN, GPIO_IN);
    gpio_init(RFID_ACTIVE_PIN);
    gpio_set_dir(RFID_ACTIVE_PIN, GPIO_OUT);
    alarm_init();          // Initialize GPIO pin for the alarm
    servo_init(SERVO_PIN); // Initialize PWM for the servo motor
}

void scan_keyboard()
{
    lcd_clear();
    const char *message = "Enter Passcode";
    int message_length = strlen(message);
    int start_position = (MAX_CHARS - message_length) / 2;
    lcd_set_cursor(0, start_position);
    lcd_print_string(message);
    for (;;)
    {
        if (!accessGranted)
        {
            char key = scanKeypad();
            if (key != '\0')
            {
                lcd_clear();
                if (key == 'K')
                {
                    passcode_length = 0;
                    passcode[0] = '\0';
                    lcd_set_cursor(0, start_position);
                }
                else if (key == 'E')
                {
                    if (strncmp(passcode, "0000", 4) == 0)
                    {
                        lcd_set_cursor(0, 0);
                        lcd_print_string("Access Granted");
                        accessGranted = true;
                        return;
                    }
                    else
                    {
                        lcd_set_cursor(0, 0);
                        lcd_print_string("Access Denied");
                        alarm_on();
                        sleep_ms(1000);
                        alarm_off();
                        accessGranted = false;
                    }
                }
                else
                {
                    if (passcode_length < 4)
                    {
                        passcode[passcode_length] = key;
                        passcode_length++;
                        passcode[passcode_length] = '\0';
                    }
                    message_length = strlen(passcode);
                    start_position = (MAX_CHARS - message_length) / 2;
                    lcd_set_cursor(0, start_position);
                    lcd_print_string(passcode);
                }
            }
        }
        sleep_ms(1000);
    }
}

void getDistance()
{
    lcd_clear();
    float distance = measure_distance();
    lcd_set_cursor(0, 0);
    char distance_str[16];
    snprintf(distance_str, sizeof(distance_str), "%.2f cm", distance);
    lcd_print_string(distance_str);
    printf("Distance: %.2f cm\n", distance);
    if (distance < 20)
    {
        alarm_on();
        servo_set_angle(SERVO_PIN, 0);
        sleep_ms(1000);
        servo_set_angle(SERVO_PIN, 90);
        sleep_ms(1000);
    }
    else
    {
        alarm_off();
    }
}

void vEntryTask(void *pvParameters)
{
    int start_position = (MAX_CHARS - passcode_length) / 2;
    for (;;)
    {
        if (!accessGranted)
        {
            if(passcode_length == 0 && !readCard)
            {
                lcd_clear();
                const char *message = "Enter Passcode";
                int message_length = strlen(message);
                start_position = (MAX_CHARS - message_length) / 2;
                lcd_set_cursor(0, start_position);
                lcd_print_string(message);
            }
            char key = scanKeypad();
            if (key != '\0')
            {
                lcd_clear();
                if (key == 'K')
                {
                    passcode_length = 0;
                    passcode[0] = '\0';
                    lcd_set_cursor(0, start_position);
                }
                else if (key == 'E')
                {
                    if (strncmp(passcode, "0000", 4) == 0)
                    {
                        lcd_set_cursor(0, 0);
                        lcd_print_string("Access Granted");
                        accessGranted = true;
                    }
                    else
                    {
                        lcd_set_cursor(0, 0);
                        lcd_print_string("Access Denied");
                        alarm_on();
                        sleep_ms(1000);
                        alarm_off();
                        accessGranted = false;
                    }
                }
                else
                {
                    if (passcode_length < 4)
                    {
                        passcode[passcode_length] = key;
                        passcode_length++;
                        passcode[passcode_length] = '\0';
                    }
                    int message_length = strlen(passcode);
                    start_position = (MAX_CHARS - message_length) / 2;
                    lcd_set_cursor(0, start_position);
                    lcd_print_string(passcode);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
    vTaskDelete(NULL);
}

void vDistanceTask(void *pvParameters)
{
    for (;;)
    {
        if (accessGranted)
        {
            lcd_clear();
            float distance = measure_distance();
            lcd_set_cursor(0, 0);
            char distance_str[16];
            snprintf(distance_str, sizeof(distance_str), "%.2f cm", distance);
            lcd_print_string(distance_str);
            printf("Distance: %.2f cm\n", distance);
            if (distance < 20)
            {
                alarm_on();
                servo_set_angle(SERVO_PIN, 0);
                sleep_ms(1000);
                servo_set_angle(SERVO_PIN, 90);
                sleep_ms(1000);
            }
            else
            {
                alarm_off();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
    vTaskDelete(NULL);
}

void vDetectionTask(void *pvParameters)
{
    for (;;)
    {
        if (!accessGranted)
        {
            cardDetection();
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
    vTaskDelete(NULL);
}

int main(void)
{
    init();

    sleep_ms(200);

    lcd_clear();
    const char *message = "Enter Passcode";
    int message_length = strlen(message);
    int start_position = (MAX_CHARS - message_length) / 2;
    lcd_set_cursor(0, start_position);
    lcd_print_string(message);

    xTaskCreate(vEntryTask, "Entry Task", 256, NULL, 1, NULL);
    xTaskCreate(vDistanceTask, "Distance Task", 256, NULL, 1, NULL);
    xTaskCreate(vDetectionTask, "Detection Task", 256, NULL, 1, NULL);

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    while (true)
    {
    }
}