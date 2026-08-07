#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "game/gamestate.h"

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
/**
 * @brief Initialise I2C communication for Display Driver, sets Brightness
 * @param sda_pin Pin connected to the SDA of RP2040
 * @param scl_pin Pin connected to the SCL of RP2040
 */
void display_init(uint8_t sda_pin, uint8_t scl_pin);

/**
 * @brief Display 4 digit number that is pass to the function on the display
 * @param value 4 digit number to be displayed
 */
void display_number(int value);

/**
 * @brief Blank all segments on the display
 */
void display_clear();

/**
 * @brief Clear range of digits on the display while keeping other numbers the same
 * @param first_digit First digit in the range to be cleared
 * @param last_digit Last digit in the range to be cleared
 */
void display_clear_digits(uint8_t first_digit, uint8_t last_digit);

/**
 * @brief Display player score, based off 2x 2 digit numbers
 * @param player_1_score Current player 1 score to be displayed
 * @param player_2_score Current player 2 score to be displayed
 */
void display_player_score(uint8_t player_1_score, uint8_t player_2_score);

/**
 * @brief Display a word on the display
 * @param word Word to be displayed (must be valid characters)
 */
void display_word(const char *word);

/**
 * @brief Blank chosen player's score, to be used for changing scores
 * @param player_number Player selected to be blanked
 */
void display_clear_individual_score(uint8_t player_number);

/**
 * @brief Display the middle segment only for all digits on the display
 */
void display_flat_lines();

/**
 * @brief Display the current mode as a word on the display
 * @param mode Selected mode
 */
void display_mode(GameMode mode);

/**
 * @brief Display a loading screen on the digital display
 * @param wait_time time (in ms) between each segment appearing on the screen
 */
void display_loading(int wait_time);