#include <stdio.h>
#include "pico/stdlib.h"

#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"
#include "helpers/colours/colours.h"

void flash_leds_rainbow(uint time_interval_ms)
{
    for (uint i = 0; i < 7; i++)
    {
        set_all_leds(get_rgb(rainbow[i]));
        update_all_leds();
        sleep_ms(time_interval_ms);
    }
}

void referee_point_scored(ServoPosition side_scored)
{
    move_motor_position_safely(side_scored);
    if (side_scored == LEFT)
    {
        set_single_led(0, get_rgb(GREEN));
    }
    if (side_scored == RIGHT)
    {
        set_single_led(1, get_rgb(GREEN));
    }
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
