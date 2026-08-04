#include "setup_round.h"

void run_setup_round_mode()
{
    uint8_t total_points = State.player_score[PLAYER_1] + State.player_score[PLAYER_2];
    State.player_serving = (State.game_number + total_points / State.settings.serves_per_player) % 2; // PLAYER_1 = 0, PLAYER_2 = 1
    printf("Player %d serving\r\n", State.player_serving + 1);
    // referee_indicate_server();
    State.ball_location = State.player_serving;
    State.ball_is_center = false;
    State.mode = SERVE_DETECTION;
}