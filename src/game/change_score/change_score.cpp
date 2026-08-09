#include <stdio.h>

#include "pico/stdlib.h"

#include "change_score.h"

#include "drivers/user_buttons/user_buttons.h"
#include "drivers/display/display.h"
#include "drivers/buzzer/buzzer.h"
#include "drivers/leds/leds.h"

#include "game/gamestate.h"
#include "programs/referee_reactions/referee_reactions.h"

#define SCORE_BLINK_INTERVAL_MS 300

#define MAX_DISPLAY_SCORE 99

// Starting selected player, will be overwritten when selecting different player
uint8_t selected_player = PLAYER_1;

// Show score if visibility is enabled
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

void run_change_score_phase()
{
    buzzer_play_select_pressed();
    clear_all_leds();     // Clear any leds indicating point or serve
    display_loading(125); // Transition to new display mode

    uint8_t max_score = State.settings.points_to_win;
    wait_for_select_release();
    bool selected_score_visible = true;
    absolute_time_t next_blink_time = make_timeout_time_ms(SCORE_BLINK_INTERVAL_MS);
    show_score_selection(selected_score_visible);

    log(LogLevel::INFORMATION, "Player select active");
    while (!is_button_pressed(SELECT_BUTTON))
    {
        if (is_button_pressed(LEFT_BUTTON))
        {
            selected_player = PLAYER_1;
            selected_score_visible = true;

            printf("PLAYER 1\n");
            show_score_selection(true);

            next_blink_time = make_timeout_time_ms(SCORE_BLINK_INTERVAL_MS);
        }
        if (is_button_pressed(RIGHT_BUTTON))
        {
            selected_player = PLAYER_2;
            selected_score_visible = true;

            printf("PLAYER 2\n");
            show_score_selection(true);

            next_blink_time = make_timeout_time_ms(SCORE_BLINK_INTERVAL_MS);
        }
        if (time_reached(next_blink_time))
        {
            selected_score_visible = !selected_score_visible;

            show_score_selection(selected_score_visible);

            next_blink_time = make_timeout_time_ms(SCORE_BLINK_INTERVAL_MS);
        }
    }

    show_score_selection(true);
    buzzer_play_select_pressed();

    log(LogLevel::INFORMATION, "Score change");
    while (!is_button_pressed(SELECT_BUTTON))
    {
        uint8_t current_selected_score = State.player_score[selected_player];
        uint8_t other_player_score = State.player_score[(selected_player == PLAYER_1) ? PLAYER_2 : PLAYER_1];
        if (is_button_pressed(LEFT_BUTTON))
        {
            if (current_selected_score != 0)
            {
                State.player_score[selected_player]--;
                display_player_score(State.player_score[PLAYER_1], State.player_score[PLAYER_2]);
                log(LogLevel::INFORMATION, "Score decrease");
            }
            else
            {
                referee_angry(1000);
            }
        }

        if (is_button_pressed(RIGHT_BUTTON))
        {

            if ((State.settings.win_by_two &&
                current_selected_score >= other_player_score + 2 &&
                current_selected_score >= max_score) 
                || 
                (!State.settings.win_by_two &&
                (current_selected_score == max_score || 
                (other_player_score == max_score &&
                current_selected_score == max_score - 1))))
            {
                referee_angry(1000);
            }
            else {
                State.player_score[selected_player]++;
                display_player_score(State.player_score[PLAYER_1], State.player_score[PLAYER_2]);
                log(LogLevel::INFORMATION, "Score increase");
            }
        }
    }
    buzzer_play_select_pressed();
    State.phase = CHECK_VICTORY_AND_SCORE;
}
