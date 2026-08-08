#pragma once

#include "pico/stdlib.h"
#include "stdio.h"

/**
 * @brief This function allows for test mode changes during sleeping
 * @param ms, milliseconds to sleep for (will round down to multiple of 10)
 * @param expected_mode expected mode when called
 * @return Returns true if mode changed, false otherwise
 */
bool sleep_ms_with_test_mode_checking(uint16_t ms, uint8_t expected_mode);

/**
 * @brief This function allows for aknowledging button interrupts and resetting display, motor position, leds and sounds
 * @param ms, milliseconds to sleep for (will round down to multiple of 10)
 * @return Returns true if mode changed, false otherwise
 */
bool sleep_ms_with_button_checking(uint16_t ms);
