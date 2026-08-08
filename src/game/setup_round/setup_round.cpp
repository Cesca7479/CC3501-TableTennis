#include "setup_round.h"
#include "programs/referee_reactions/referee_reactions.h"
#include "helpers/dc_bias/dc_bias.h"

void run_setup_round_phase()
{
    uint8_t total_points = State.player_score[PLAYER_1] + State.player_score[PLAYER_2];
    State.player_serving = (State.game_number + total_points / State.settings.serves_per_player) % 2; // PLAYER_1 = 0, PLAYER_2 = 1
    printf("Player %d serving\r\n", State.player_serving + 1);
    referee_indicate_server(State.player_serving);
    State.ball_location = State.player_serving;
    State.ball_is_center = false;
    State.phase = SERVE_DETECTION;

    set_dc_biases();
}