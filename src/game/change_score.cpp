#include <stdio.h>

#include "pico/stdlib.h"

#include "change_score.h"
#include "drivers/user_buttons/user_buttons.h"
#include "drivers/display/display.h"
#include "game/gamestate.h"

#define SCORE_BLINK_INTERVAL_MS 300

uint8_t selected_player = PLAYER_1;

enum class ButtonControllerMode
{
    NONE,
    SCORE_SELECT_PLAYER,
    SCORE_EDIT_VALUE
};

static void wait_for_select_release()
{
    // Clear the event that opened this mode
    (void)is_button_pressed(SELECT_BUTTON);

    // Do not continue until the physical button is released
    while (is_button_on(SELECT_BUTTON))
    {
        sleep_ms(1);
    }

    // Clear any bounce event produced during release
    (void)is_button_pressed(SELECT_BUTTON);
}

static void show_score_selection(bool visible)
{
    display_player_score(
        State.player_score[PLAYER_1],
        State.player_score[PLAYER_2]);

    if (!visible)
    {
        display_clear_individual_score(selected_player);
    }
}

void run_change_score_mode()
{
    wait_for_select_release();
    bool selected_score_visible = true;
    absolute_time_t next_blink_time = make_timeout_time_ms(SCORE_BLINK_INTERVAL_MS);
    show_score_selection(selected_score_visible);

    // Select player to change
    printf("SELECT PLAYER\n");
    while (!is_button_pressed(SELECT_BUTTON))
    {
        if (is_button_pressed(LEFT_BUTTON))
        {
            selected_player = PLAYER_1;
            selected_score_visible = true;

            printf("PLAYER 1\n");
            show_score_selection(true);

            next_blink_time =
                make_timeout_time_ms(SCORE_BLINK_INTERVAL_MS);
        }
        if (is_button_pressed(RIGHT_BUTTON))
        {
            selected_player = PLAYER_2;
            selected_score_visible = true;

            printf("PLAYER 2\n");
            show_score_selection(true);

            next_blink_time =
                make_timeout_time_ms(SCORE_BLINK_INTERVAL_MS);
        }
        if (time_reached(next_blink_time))
        {
            selected_score_visible =
                !selected_score_visible;

            show_score_selection(
                selected_score_visible);

            next_blink_time =
                make_timeout_time_ms(
                    SCORE_BLINK_INTERVAL_MS);
        }
    }

    Piezos[2].play_select();

    // Change score
    printf("CHANGE SCORE\n");
    while (!is_button_pressed(SELECT_BUTTON))
    {
        if (is_button_pressed(LEFT_BUTTON))
        {
            State.player_score[selected_player]--;
            printf("SCORE DECREASED\n");
        }
        if (is_button_pressed(RIGHT_BUTTON))
        {
            State.player_score[selected_player]++;
            printf("SCORE INCREASED\n");
        }
        display_player_score(State.player_score[PLAYER_1], State.player_score[PLAYER_2]);
    }

    Piezos[2].play_select();
    State.mode = CHECK_VICTORY_AND_SCORE;
}
