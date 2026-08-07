#pragma once

#include "pico/stdlib.h"
#include "stdio.h"
#include "drivers/piezos/piezos.h"
#include "drivers/display/display.h"
#include "game/gamestate.h"
#include <cstring>
#include <iterator>
#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"
#include "helpers/colours/colours.h"
#include "programs/referee_reactions/referee_reactions.h"

extern uint8_t mode;
extern bool mode_change_logged;

enum Mode
{
    DEFAULT_MODE,
    PIEZO_TEST_MODE,
    DISPLAY_TEST_MODE,
    BLUETOOTH_TEST_MODE,
    MOTOR_TEST_MODE,
    LED_TEST_MODE,
    REFEREE_TEST_MODE,
    HAT_ID_TEST_MODE,
    USER_BUTTON_TEST_MODE,
    MUSIC_TEST_MODE,
    NUM_MODES
};

void on_board_button_callback(uint gpio, uint32_t events);

/**
 * @brief This function allows for mode changes during sleeping
 * @param ms, milliseconds to sleep for (will round down to multiple of 10)
 * @param expected_mode expected mode when called
 * @return Returns true if mode changed, false otherwise
 */
bool sleep_ms_with_checking(uint16_t ms, uint8_t expected_mode);

void run_default_mode();
void run_piezo_test_mode();
void run_motor_test_mode();
void run_led_test_mode();
void run_referee_test_mode();
void run_hat_id_test_mode();
void run_user_buttons_test_mode();

/**
 * @brief Test display functionality, also test bluetooth functionality
 */
void run_display_test_mode();

/**
 * @brief Test sending player scores to the display
 */
void run_display_test_scores();

/**
 * @brief Test letters to be sent through to the display
 */
void run_test_letters();

/**
 * @brief Test sending words to the display
 */
void run_test_words();

/**
 * @brief Test buzzer and tunes
 */
void run_music_test_mode();