#include <stdint.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "hardware/gpio.h"
#include "hardware/sync.h"

#include "user_buttons.h"
#include "game/gamestate.h"
#include "board.h"

#define BUTTON_DEBOUNCE_TIME_US 10000
#define NUMBER_OF_BUTTONS 3

// released = LOW, pressed  = HIGH

uint8_t BUTTON_PINS[] = {
    SW1_PIN,
    SW2_PIN,
    SW3_PIN};

struct ButtonState
{
    volatile bool is_button_pressed;
    volatile uint32_t time_since_last_press;
};

ButtonState button_states[NUMBER_OF_BUTTONS]{};

void button_callback(uint gpio, uint32_t event_mask)
{
    for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++)
    {
        if (BUTTON_PINS[i] == gpio)
        {
            // check for 'button bounce'
            uint32_t current_time_us = time_us_32();
            if (current_time_us - button_states[i].time_since_last_press > 10000) // 10000us = 10ms
            {
                button_states[i].time_since_last_press = current_time_us;
                button_states[i].is_button_pressed = true;

                if (State.mode != CHANGE_SCORE)
                {
                    State.mode = CHANGE_SCORE;
                }
            }
        }
    }
}

void user_buttons_init()
{
    for (uint8_t i = 0; i < NUMBER_OF_BUTTONS; i++)
    {
        uint8_t pin = BUTTON_PINS[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE, true, &button_callback);

        button_states[i].is_button_pressed = false;
        button_states[i].time_since_last_press = 0;
    }
}

bool is_button_pressed(UserButton button)
{
    // Ensure button interrupt doesn't override current button press
    uint32_t interrupt_state = save_and_disable_interrupts();
    bool was_pressed = button_states[button].is_button_pressed;
    button_states[button].is_button_pressed = false;
    restore_interrupts(interrupt_state);
    return was_pressed;
}

bool is_button_on(UserButton button)
{
    uint8_t index = static_cast<uint8_t>(button);

    if (index >= NUMBER_OF_BUTTONS)
    {
        return false;
    }

    return gpio_get(BUTTON_PINS[index]);
}

const char *user_button_to_string(UserButton button)
{
    switch (button)
    {
    case LEFT_BUTTON:
        return "LEFT BUTTON";

    case SELECT_BUTTON:
        return "CENTRE BUTTON";

    case RIGHT_BUTTON:
        return "RIGHT BUTTON";

    default:
        return "UNKNOWN BUTTON";
    }
}