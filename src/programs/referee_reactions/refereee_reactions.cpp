#include <stdio.h>
#include "pico/stdlib.h"

#include "referee_reactions.h"
#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"
#include "helpers/colours/colours.h"
#include "drivers/logging/logging.h"
#include "drivers/piezos/piezos.h"

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
    Piezos[2].init_buzzer();
    Piezos[2].play_tone(notes6[0]);
    sleep_ms(200);
    Piezos[2].play_tone(notes6[2]);
    motor_move_motor_safely(side_serving);
    light_player_side(side_serving, get_rgb(WHITE));
    Piezos[2].play_tone(notes6[4]);
    sleep_ms(200);
    Piezos[2].stop_buzzer();
}

void referee_point_scored(ServoPosition side_scored)
{
    Piezos[2].init_buzzer();
    Piezos[2].play_tone(notes6[6]);
    motor_move_motor_safely(side_scored);
    light_player_side(side_scored, get_rgb(GREEN));
    Piezos[2].play_tone(notes7[2]);
    sleep_ms(200);
    Piezos[2].stop_buzzer();
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
    absolute_time_t end_time = make_timeout_time_ms(duration_ms);
    while (!time_reached(end_time))
    {
        Piezos[2].init_buzzer();
        // motor_move_motor_safely(LEFT);
        motor_set_position(LEFT);
        motor_enable();
        set_all_leds(get_rgb(RED));
        update_all_leds();
        for (uint8_t i = 0; i < 5; i++)
        {
            Piezos[2].play_tone(notes7[0]);
            sleep_ms(20);
            Piezos[2].play_tone(notes6[6]);
            sleep_ms(20);
        }
        motor_disable();

        // motor_move_motor_safely(RIGHT);
        motor_set_position(RIGHT);
        motor_enable();
        set_all_leds(get_rgb(OFF));
        update_all_leds();
        for (uint8_t i = 0; i < 5; i++)
        {
            Piezos[2].play_tone(notes7[0]);
            sleep_ms(20);
            Piezos[2].play_tone(notes6[6]);
            sleep_ms(20);
        }
        Piezos[2].stop_buzzer();
    }
    motor_move_motor_safely(CENTRE);
}
