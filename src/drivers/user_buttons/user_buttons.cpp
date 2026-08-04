#include <stdio.h>

#include "pico/stdlib.h"

#include "user_buttons.h"
#include "board.h"

#define BUTTON_DEBOUNCE_TIME_US 10000
#define NUMBER_OF_BUTTONS 3

// released = LOW, pressed  = HIGH
// Uses polling instead of interrupts

uint8_t BUTTON_PINS[] = {
    SW1_PIN,
    SW2_PIN,
    SW3_PIN};

struct ButtonState
{
    bool is_last_state_on; // debounce adjusted state
    bool pressed;
    bool released;
    uint64_t change_time_us;
};

ButtonState button_states[NUMBER_OF_BUTTONS]{};

void user_buttons_init()
{
    uint64_t start_time = to_us_since_boot(get_absolute_time());

    for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++)
    {
        uint8_t pin = BUTTON_PINS[i];

        // Initialise pin
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);

        // Initialise button states
        button_states[i].is_last_state_on = 0;
        button_states[i].change_time_us = start_time;
        button_states[i].pressed = false;
        button_states[i].released = false;
    }
}

void user_buttons_update()
{
    uint64_t current_time = to_us_since_boot(get_absolute_time());

    for (size_t i = 0; i < NUMBER_OF_BUTTONS; i++)
    {
        ButtonState &current_state = button_states[i];
        bool is_state_on = gpio_get(BUTTON_PINS[i]);

        // Check if current button state has been changed ignoring debounce
        if (is_state_on != current_state.is_last_state_on && (current_time - current_state.change_time_us) >= BUTTON_DEBOUNCE_TIME_US)
        {
            current_state.is_last_state_on = is_state_on;
            current_state.change_time_us = current_time;

            // Set released or pressed states
            if (is_state_on)
            {
                current_state.pressed = true;
            }
            else
            {
                current_state.released = true;
            }
        }
    }
}

bool is_button_on(UserButton button)
{
    return button_states[button].is_last_state_on;
}

bool is_button_pressed(UserButton button)
{
    ButtonState &state = button_states[button];

    bool event_occurred = state.pressed;
    state.pressed = false;

    return event_occurred;
}

bool is_button_released(UserButton button)
{
    ButtonState &state = button_states[button];

    bool event_occurred = state.released;
    state.released = false;

    return event_occurred;
}