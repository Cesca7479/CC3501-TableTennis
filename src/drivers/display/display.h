#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"

#define I2C_PORT i2c0
#define HT16K33_ADDR 0x70
#define OSCILLATOR_ON 0x21
#define DISPLAY_ON 0x81
#define BRIGHTNESS 255

const uint8_t digits[] =
    {
        0b00111111, // 0
        0b00000110, // 1
        0b01011011, // 2
        0b01001111, // 3
        0b01100110, // 4
        0b01101101, // 5
        0b01111101, // 6
        0b00000111, // 7
        0b01111111, // 8
        0b01101111  // 9
};

void set_up_display(uint8_t sda_pin, uint8_t scl_pin);
void display_number(int value);
void clear_display();
void blank_digits(uint8_t first_digit, uint8_t last_digit);