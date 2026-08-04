#include "check_victory_and_score.h"

void run_check_victory_and_score_mode()
{
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
        State.mode = SETUP_ROUND;
    }

    if (is_win) {
        printf("Player %d has won!\r\n", winner + 1);
        State.mode = SETUP_GAME;
    }
    // referee_dance();
    return;
}