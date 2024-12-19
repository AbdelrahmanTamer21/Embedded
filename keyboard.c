#include "keyboard.h"

const uint8_t rowPins[ROWS] = {20, 21, 22, 26}; // Pins for rows
const uint8_t colPins[COLS] = {19, 18, 17, 16};   // Pins for columns

// Keypad matrix
const char hexaKeys[ROWS][COLS] = {
    {'M', '2', '3', 'A'},
    {'1', '5', '6', 'B'},
    {'4', '8', '9', 'C'},
    {'7', '0', 'K', 'E'}
};


void gpio_init_keypad() {
    // Initialize rows as outputs and set them HIGH
    for (int i = 0; i < ROWS; i++) {
        gpio_init(rowPins[i]);
        gpio_set_dir(rowPins[i], GPIO_OUT);
        gpio_put(rowPins[i], 1); // Set rows HIGH initially
    }

    // Initialize columns as inputs with pull-ups
    for (int i = 0; i < COLS; i++) {
        gpio_init(colPins[i]);
        gpio_set_dir(colPins[i], GPIO_IN);
        gpio_pull_up(colPins[i]); // Enable pull-up resistor
    }
}

char scanKeypad(void) {
    for (int row = 0; row < ROWS; row++) {
        // Activate the current row by setting it LOW
        gpio_put(rowPins[row], 0);

        for (int col = 0; col < COLS; col++) {
            // Check if the column pin is LOW (key pressed)
            if (gpio_get(colPins[col]) == 0) {
                // Wait until the key is released
                while (gpio_get(colPins[col]) == 0);
                gpio_put(rowPins[row], 1); // Reset the row
                return hexaKeys[row][col]; // Return the pressed key
            }
        }

        // Reset the row by setting it HIGH
        gpio_put(rowPins[row], 1);
    }

    return '\0'; // No key pressed
}
