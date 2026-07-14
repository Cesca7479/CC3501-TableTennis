#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
// Need to include the adc one.

// #include "drivers/leds.h"
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file
#include "drivers/logging/logging.h"
// #include "arm_math.h"

#define NUM_LEDS 5 // Change this
#define ON_BOARD_BUTTON 22
#define LED_PIN 5
#define LEFT_BUTTON 19
#define SELECT_BUTTON 20
#define RIGHT_BUTTON 21

// For the led display
#define I2C_PORT i2c0
#define SCL_PIN 17
#define SDA_PIN 16
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

enum mode
{
    RESET,
    START,
    STOP,
    NUM_MODES
};