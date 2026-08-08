#include "timing.h"

#include "tests/testing.h"
#include "drivers/user_buttons/user_buttons.h"
#include "drivers/display/display.h"
#include "drivers/buzzer/buzzer.h"
#include "drivers/motor/motor.h"

bool sleep_ms_with_button_checking(uint16_t ms)
{
    for (size_t i = 0; i < (ms / 10); i++)
    {
        sleep_ms(10);
        if (is_button_pressed(SELECT_BUTTON) || is_button_pressed(LEFT_BUTTON) || is_button_pressed(RIGHT_BUTTON)) {
            buzzer_stop();
            motor_move_motor_safely(CENTRE);
            display_word("stop");
            sleep_ms(500);
            display_player_score(State.player_score[0], State.player_score[1]);
            return true;
        }
    }
    return false;
}

bool sleep_ms_with_test_mode_checking(uint16_t ms, uint8_t expected_mode)
{
    for (size_t i = 0; i < (ms / 10); i++)
    {
        sleep_ms(10);
        if (mode != expected_mode)
            return true;
    }
    return false;
}