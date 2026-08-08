#include <stdio.h>
#include "pico/stdlib.h"

#include "referee_reactions.h"
#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"
#include "helpers/colours/colours.h"
#include "drivers/logging/logging.h"
#include "drivers/piezos/piezos.h"
#include "drivers/buzzer/buzzer.h"

#include "helpers/timing/timing.h"

#include "game/gamestate.h"

uint8_t RAINBOW_COLOUR_COUNT = 7;
uint32_t DANCE_COLOUR_INTERVAL_MS = 75;
uint32_t DANCE_UPDATE_INTERVAL_MS = 5;

const uint32_t notes5[7] = {523, 587, 659, 698, 784, 879, 988};
const uint32_t notes6[7] = {1047, 1175, 1319, 1397, 1568, 1760, 1976};
const uint32_t notes7[7] = {2093, 2349, 2637, 2794, 3136, 3520, 3951};

/**
 * @brief Perform one dance move, which moves arm once and changes colour 4 times
 * @param position Side to move motor to
 * @param duration_ms The duration of the dance in milliseconds
 * @param rainbow_index Reference to rainbow index
 * @param move_time Time for one dance move (time to move motor once)
 */
static bool perform_dance_move(ServoPosition position, absolute_time_t duration_ms, uint8_t &rainbow_index, uint move_time)
{
    motor_set_position(position);

    if (!motor_enable())
    {
        return false;
    }

    absolute_time_t move_end = make_timeout_time_ms(move_time);

    absolute_time_t next_colour_time = get_absolute_time(); // Change the first colour immediately.
    while (!time_reached(move_end) && !time_reached(duration_ms))
    {
        if (is_motor_fault_active())
        {
            log(LogLevel::ERROR, "Motor fault, over-current or high temperature detected. Motor power disabled.");
            motor_disable();
            return false;
        }

        if (time_reached(next_colour_time))
        {
            set_all_leds(get_rgb(rainbow[rainbow_index]));
            update_all_leds();

            rainbow_index = (rainbow_index + 1) % RAINBOW_COLOUR_COUNT;

            next_colour_time = make_timeout_time_ms(DANCE_COLOUR_INTERVAL_MS);
        }

        if (sleep_ms_with_button_checking(DANCE_UPDATE_INTERVAL_MS))
            return false;
    }
    motor_disable();
    return true;
}

/**
 * @brief Referee reaction for a dance
 * @param duration_ms The duration of the dance in milliseconds
 * @param move_time Time for each movement in perform_dance_move
 */
static void referee_dance(uint duration_ms, uint move_time)
{
    absolute_time_t dance_end = make_timeout_time_ms(duration_ms);
    ServoPosition position = LEFT;
    uint8_t rainbow_index = 0;

    while (!time_reached(dance_end))
    {
        if (!perform_dance_move(position, dance_end, rainbow_index, move_time))
        {
            break;
        }
        position = (position == LEFT) ? RIGHT : LEFT;
    }
    clear_all_leds();
}

/**
 * @brief Flashes LEDs in a rainbow pattern
 *
 * Note: move this function to an appropriate file in the future, as it is not specific to referee reactions
 * @param time_interval_ms The time interval between each color change in milliseconds
 */
static void flash_leds_rainbow(uint time_interval_ms)
{
    for (uint i = 0; i < 7; i++)
    {
        set_all_leds(get_rgb(rainbow[i]));
        update_all_leds();
        if (sleep_ms_with_button_checking(time_interval_ms))
            return;
    }
}

/**
 * @brief Light led on side of specified player
 * @param player_side The side the specified player is on
 * @param colour The colour to light the led
 */
static void light_player_side(ServoPosition player_side, rgb_colour colour)
{
    if (player_side == LEFT)
    {
        set_single_led(1, colour);
    }
    if (player_side == RIGHT)
    {
        set_single_led(0, colour);
    }
    update_all_leds();
}

void referee_indicate_server(ServoPosition side_serving)
{
    buzzer_play_tone(notes6[0]);
    if (sleep_ms_with_button_checking(200))
        return;
    buzzer_play_tone(notes6[2]);
    motor_move_motor_safely(side_serving);
    light_player_side(side_serving, get_rgb(WHITE));
    buzzer_play_tone(notes6[4]);
    if (sleep_ms_with_button_checking(200))
        return;
    buzzer_stop();
    if (sleep_ms_with_button_checking(500))
        return;
    motor_move_motor_safely(CENTRE);
    // needs to drop arm and turn light off after ball bounce
}

void referee_point_scored(ServoPosition side_scored)
{
    buzzer_play_tone(notes6[6]);
    motor_move_motor_safely(side_scored);
    light_player_side(side_scored, get_rgb(GREEN));
    buzzer_play_tone(notes7[2]);
    if (sleep_ms_with_button_checking(200))
        return;
    buzzer_stop();
    motor_move_motor_safely(CENTRE);
    // Needs to drop arm and turn light off atp
}

void referee_dance_sequence()
{
    buzzer_play_victory_sequence();
    referee_dance(1000, 500);
    referee_dance(500, 250);
    if (sleep_ms_with_button_checking(250))
        return;
    referee_dance(1000, 500);
    referee_dance(500, 250);
    if (sleep_ms_with_button_checking(250))
        return;
    motor_move_motor_safely(CENTRE);
    // Incorporate sound and movement
}

void referee_angry(uint duration_ms)
{
    set_all_leds(get_rgb(RED));
    update_all_leds();
    buzzer_play_angry_sounds();
    absolute_time_t end_time = make_timeout_time_ms(duration_ms);
    while (!time_reached(end_time))
    {
        motor_move_motor_safely(LEFT);
        set_all_leds(get_rgb(RED));
        update_all_leds();
        motor_disable();

        motor_move_motor_safely(RIGHT);
        set_all_leds(get_rgb(OFF));
        update_all_leds();
        for (uint8_t i = 0; i < 5; i++)
        {
            buzzer_play_tone(notes7[0]);
            if (sleep_ms_with_button_checking(20))
                return;
            buzzer_play_tone(notes6[6]);
            if (sleep_ms_with_button_checking(20))
                return;
        }
    }
    motor_move_motor_safely(CENTRE);
    buzzer_stop();
}