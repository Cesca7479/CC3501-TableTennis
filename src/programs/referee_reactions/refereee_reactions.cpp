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
    motor_move_motor_safely(side_serving);
    light_player_side(side_serving, get_rgb(WHITE));
}

void referee_point_scored(ServoPosition side_scored)
{
    motor_move_motor_safely(side_scored);
    light_player_side(side_scored, get_rgb(GREEN));
}

void referee_dance(uint duration_ms)
{
    absolute_time_t end_time = make_timeout_time_ms(duration_ms);
    while (!time_reached(end_time))
    {
        motor_move_motor_safely(LEFT);
        flash_leds_rainbow(10);

        motor_move_motor_safely(RIGHT);
        flash_leds_rainbow(10);
    }
}

void referee_angry(uint duration_ms)
{
    absolute_time_t end_time = make_timeout_time_ms(duration_ms);
    while (!time_reached(end_time))
    {
        motor_move_motor_safely(LEFT);
        set_all_leds(get_rgb(RED));
        update_all_leds();

        motor_move_motor_safely(RIGHT);
        set_all_leds(get_rgb(OFF));
        update_all_leds();
    }
    motor_move_motor_safely(CENTRE);
}
