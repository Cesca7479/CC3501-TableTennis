#include "check_victory_and_score.h"

void run_check_victory_and_score_mode()
{
    display_player_score(State.player_score[PLAYER_1], State.player_score[PLAYER_2]); // DISPLAY POINTS
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
        State.mode = SETUP_ROUND;
    }

    if (is_win) {
        printf("Player %d has won!\r\n", winner + 1);
        State.mode = SETUP_GAME;
    }
    // referee_dance();
    return;
}