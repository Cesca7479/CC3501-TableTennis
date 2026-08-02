#include <stdio.h>
#include "pico/stdlib.h"

#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"
#include "helpers/colours/colours.h"
#include "referee_reactions.h"

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
        set_single_led(0, colour);
    }
    if (player_side == RIGHT)
    {
        set_single_led(1, colour);
    }
}

void referee_indicate_server(ServoPosition side_serving)
{
    move_motor_position_safely(side_serving);
    light_player_side(side_serving, get_rgb(WHITE));
}

void referee_point_scored(ServoPosition side_scored)
{
    move_motor_position_safely(side_scored);
    light_player_side(side_scored, get_rgb(GREEN));
}

void referee_dance(uint duration_ms)
{
    absolute_time_t end_time = make_timeout_time_ms(duration_ms);
    while (!time_reached(end_time))
    {
        move_motor_position_safely(LEFT);
        flash_leds_rainbow(10);

        move_motor_position_safely(RIGHT);
        flash_leds_rainbow(10);
    }
}

void referee_angry(ServoPosition bad_side, uint duration_ms)
{
    absolute_time_t end_time = make_timeout_time_ms(duration_ms);
    while (!time_reached(end_time))
    {
        move_motor_position_safely(bad_side, 50);
        set_all_leds(get_rgb(RED));
        update_all_leds();

        move_motor_position_safely(CENTRE, 50);
        set_all_leds(get_rgb(OFF));
        update_all_leds();
    }
}
