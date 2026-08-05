#include <stdio.h>
#include "pico/stdlib.h"

#include "referee_reactions.h"
#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"
#include "helpers/colours/colours.h"
#include "drivers/logging/logging.h"
#include "drivers/piezos/piezos.h"

#include "game/gamestate.h"

uint8_t RAINBOW_COLOUR_COUNT = 7;
uint32_t DANCE_COLOUR_INTERVAL_MS = 75;
uint32_t DANCE_UPDATE_INTERVAL_MS = 5;

void flash_leds_rainbow(uint time_interval_ms)
{
    for (uint i = 0; i < 7; i++)
    {
        set_all_leds(get_rgb(rainbow[i]));
        update_all_leds();
        sleep_ms(time_interval_ms);
    }
}

void light_player_side(ServoPosition player_side, rgb_colour colour)
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
    Piezos[1].play_serve();
    motor_move_motor_safely(side_serving);
    light_player_side(side_serving, get_rgb(WHITE));
}

void referee_point_scored(ServoPosition side_scored)
{
    Piezos[1].play_point();
    motor_move_motor_safely(side_scored);
    light_player_side(side_scored, get_rgb(GREEN));
}

bool perform_dance_move(ServoPosition position, absolute_time_t duration_ms, uint8_t &rainbow_index, uint move_time)
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

        sleep_ms(DANCE_UPDATE_INTERVAL_MS);
    }
    motor_disable();
    return true;
}

void referee_dance(uint duration_ms, uint move_time)
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

void referee_angry(uint duration_ms)
{
    Piezos[1].play_angry_sounds();
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
    }
    motor_move_motor_safely(CENTRE);
}
