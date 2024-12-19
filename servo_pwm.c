#include "servo_pwm.h"
#include "hardware/pwm.h"

// Helper function to calculate the duty cycle for a given angle
static uint16_t calculate_duty_cycle(float angle) {
    // Calculate pulse width (1 ms for 0°, 2 ms for 180°)
    float pulse_width_ms = 1.0 + (angle / 180.0) * 1.0; // Map 0°-180° to 1-2ms
    return (uint16_t)((pulse_width_ms / 20.0) * 65536); // Convert to 16-bit duty cycle
}

// Initialize the servo motor on the given GPIO pin
void servo_init(uint gpio_pin) {
    // Configure the GPIO pin for PWM
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);

    // Get the PWM slice and channel associated with the GPIO pin
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);

    // Configure PWM for 50 Hz (20 ms period)
    pwm_set_wrap(slice_num, 24999); // Wrap = (125e6 / (50 * 64)) - 1
    pwm_set_clkdiv(slice_num, 64.0); // Divide clock by 64 to get ~20ms period

    // Enable PWM on the slice
    pwm_set_enabled(slice_num, true);
}

// Set the servo angle on the given GPIO pin
void servo_set_angle(uint gpio_pin, float angle) {
    // Ensure the angle is within the valid range (0° to 180°)
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    // Get the PWM slice and channel for the GPIO pin
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
    uint channel = pwm_gpio_to_channel(gpio_pin);

    // Calculate and set the PWM duty cycle for the desired angle
    uint16_t duty_cycle = calculate_duty_cycle(angle);
    pwm_set_chan_level(slice_num, channel, duty_cycle);
}
