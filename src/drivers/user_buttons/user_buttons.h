#pragma once

enum UserButton
{
    RIGHT_BUTTON,
    SELECT_BUTTON,
    LEFT_BUTTON,
};

void user_button_callback(uint gpio, uint32_t event_mask);

/**
 * @brief Initalise all user buttons
 */
void user_buttons_init();

/**
 * @brief Check if button has been pressed, then reset pressed state.
 *
 * Note: this function temporarily blocks interrupts when resetting state
 * @param button Button to check (LEFT_BUTTON, SELECT_BUTTON, RIGHT_BUTTON)
 */
bool is_button_pressed(UserButton button);

bool is_button_on(UserButton button);

/**
 * @brief Return the name of a user button
 *
 * @param button Button to get name from (LEFT_BUTTON, SELECT_BUTTON, RIGHT_BUTTON)
 */
const char *user_button_to_string(UserButton button);

void wait_for_select_release();