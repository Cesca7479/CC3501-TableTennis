#include "bounce_listen.h"
#include "programs/referee_reactions/referee_reactions.h"

void run_bounce_listening_mode()
{
    absolute_time_t current_time = get_absolute_time();

    uint16_t result;
    bool isBounce[2] = {false, false};

    for (size_t i = 0; i < 2; i++)
    {
        if (absolute_time_diff_us(State.last_read_time[i], current_time) > BOUNCE_SAMPLING_RATE_MS * 1000) {
            result = Piezos[i].read();
            isBounce[i] = result > State.piezo_dc_biases[i] + SENSITIVITY_THRESHOLD_TABLE || result < State.piezo_dc_biases[i] - SENSITIVITY_THRESHOLD_TABLE;
            if (isBounce[i]) State.last_read_time[i] = current_time;
        }
    }

    // Ignores "bounces" if they do not occur where the ball is positioned - rejects accidental contacts
    if (State.ball_location != PLAYER_1 && State.rpi_connected) isBounce[PLAYER_1] = false;
    if (State.ball_location != PLAYER_2 && State.rpi_connected) isBounce[PLAYER_2] = false;

    
    if (isBounce[PLAYER_1] && State.prev_bounce_side == PLAYER_1) // Detects double bounce in player 1 side
    {
        printf("Double bounce in Player 1 side\r\n");
        State.player_score[PLAYER_2]++;
        referee_point_scored(RIGHT);
        State.mode = CHECK_VICTORY_AND_SCORE;
    }
    else if (isBounce[PLAYER_2] && State.prev_bounce_side == PLAYER_2) // Detects double bounce in player 2 side
    {
        printf("Double bounce in Player 2 side\r\n");
        State.player_score[PLAYER_1]++;
        referee_point_scored(LEFT);
        State.mode = CHECK_VICTORY_AND_SCORE;
    }
    else if (isBounce[PLAYER_1]) // Sets previous bounce side and time
    {
        printf("Bounce in Player 1 side\r\n");
        State.prev_bounce_side = PLAYER_1;
        State.prev_bounce_time = current_time;
    }
    else if (isBounce[PLAYER_2]) // Sets previous bounce side and time
    {
        printf("Bounce in player 2 side\r\n");
        State.prev_bounce_side = PLAYER_2;
        State.prev_bounce_time = current_time;
    }
    else if (absolute_time_diff_us(State.prev_bounce_time, current_time) >= TIME_OUT_THRESHOLD_MS * 1000) // Detects ball gone out by time threshold
    {
        printf("Ball went out\r\n");
        State.player_score[(State.prev_bounce_side == PLAYER_1) ? PLAYER_2 : PLAYER_1]++;
        referee_point_scored((State.prev_bounce_side == PLAYER_1) ? LEFT : RIGHT );
        State.mode = CHECK_VICTORY_AND_SCORE;
    } 
    
    if (State.rpi_connected) {
        State.camera_check_return_to_mode = BOUNCE_LISTEN;
        State.mode = CAMERA_CHECK;
    }
}