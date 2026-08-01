#pragma once
#include "helpers/colours/colours.h"

#define LED_PIO pio0
#define LED_SM 0
#define MAX_NUM_LED 2

#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define DEFAULT_TEXT "\033[0m"

/**
 * @brief Initialise PIO to control the LED chain
 * 
 * By default, all LEDs off (0, 0, 0)
 */
void init_leds();

/**
 * @brief Set a single LED colour (RGB)
 * @param led_index Index of desired LED to set
 * @param colour rgb values of the colour to set. Use get_rgb(colour_name) to get the rgb values of a given colour name.
 */
void set_single_led(uint8_t led_index, rgb_colour colour);

/**
 * @brief Set all LED colours to one colour (RGB)
 * @param colour rgb values of the colour to set. Use get_rgb(colour_name) to get the rgb values of a given colour name.
 */
void set_all_leds(rgb_colour colour);

/**
 * @brief Print the status of all LEDs
 * 
 * If not updated, print set colours and current colours of all LEDs
 */
void print_status_leds();

/**
 * @brief Clear all LEDs to (0, 0, 0)
 * 
 * Both setting and updating all LEDs
 */
void clear_all_leds();

/** @brief Update all LEDs to set colours */
void update_all_leds();