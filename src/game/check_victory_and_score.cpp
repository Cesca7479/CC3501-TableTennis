#include "check_victory_and_score.h"
#include "drivers/user_buttons/user_buttons.h"
#include "programs/referee_reactions/referee_reactions.h"
#include "drivers/buzzer/buzzer.h"

void run_check_victory_and_score_phase()
{
    printf("Player 1: %d, Player 2: %d\r\n", State.player_score[PLAYER_1], State.player_score[PLAYER_2]);
    display_player_score(State.player_score[PLAYER_1], State.player_score[PLAYER_2]); // DISPLAY POINTS
    std::string msg;
    msg = "Player1: " + std::to_string(State.player_score[PLAYER_1]) + ", Player2: " + std::to_string(State.player_score[PLAYER_2]) + "\n";
    bluetooth_send(msg.c_str());
    bool is_win = false;
    uint8_t winner;
    if (State.player_score[PLAYER_1] >= State.settings.points_to_win && (State.player_score[PLAYER_1] - 2 >= State.player_score[PLAYER_2] || !State.settings.win_by_two))
    {
        is_win = true;
        winner = PLAYER_1;
    }
    else if (State.player_score[PLAYER_2] >= State.settings.points_to_win && (State.player_score[PLAYER_2] - 2 >= State.player_score[PLAYER_1] || !State.settings.win_by_two))
    {
        is_win = true;
        winner = PLAYER_2;
    }
    else
    {
        State.phase = SETUP_ROUND;
    }

    if (is_win)
    {
        printf("Player %d has won!\r\n", winner + 1);
        buzzer_play_victory_sequence();
        msg = "Won: Player" + std::to_string(winner) + "\n";
        referee_dance_sequence();

        // Wait for confirmation of score to continue
        while (!is_button_pressed(SELECT_BUTTON))
        {
            // Condition to change score (exception to default condition to change score)
            if (is_button_pressed(LEFT_BUTTON) || is_button_pressed(RIGHT_BUTTON))
            {
                {
                    State.phase = CHANGE_SCORE;
                    return;
                }
            }
        }

        // Confirmed to upload results and move on
        bluetooth_send(msg.c_str());
        State.game_number++;
        State.phase = SETUP_GAME;
    }
}