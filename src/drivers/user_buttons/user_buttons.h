#pragma once

enum UserButton
{
    LEFT_BUTTON,
    SELECT_BUTTON,
    RIGHT_BUTTON,
};

/**
 * @brief Initalise all user buttons
 */
void user_buttons_init();

/**
 * @brief update internal state of user buttons
 */
void user_buttons_update();

/**
 * @brief Check if button is currently on
 *
 * @param button Button to check (LEFT_BUTTON, SELECT_BUTTON, RIGHT_BUTTON)
 */
bool is_button_on(UserButton button);

/**
 * @brief Check if button has been pressed, then reset pressed state to false
 *
 * @param button Button to check (LEFT_BUTTON, SELECT_BUTTON, RIGHT_BUTTON)
 */
bool is_button_pressed(UserButton button);

/**
 * @brief Check if button has been released, then reset released state to false
 *
 * @param button Button to check (LEFT_BUTTON, SELECT_BUTTON, RIGHT_BUTTON)
 */
bool is_button_released(UserButton button);