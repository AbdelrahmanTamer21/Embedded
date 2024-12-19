#include "ultrasonic.h"

void ultrasonic_init()
{
    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

float measure_distance()
{
    // Send a 10us pulse to the trigger pin
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, 0);

    // Wait for the echo pin to go high
    while (gpio_get(ECHO_PIN) == 0);

    // Start measuring time
    absolute_time_t start_time = get_absolute_time();

    // Wait for the echo pin to go low
    while (gpio_get(ECHO_PIN) == 1);

    // Stop measuring time
    absolute_time_t end_time = get_absolute_time();

    // Calculate the duration of the pulse
    uint32_t duration = absolute_time_diff_us(start_time, end_time);

    // Calculate the distance in cm
    float distance = duration * 0.034 / 2;

    return distance;
}