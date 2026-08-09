#include "serve_detection.h"
#include "programs/referee_reactions/referee_reactions.h"
#include "drivers/leds/leds.h"
#include "helpers/scoring/scoring.h"

static void reset_serve_state(uint8_t &serve_attempts, bool &has_hit_table, bool &has_hit_net) // Resets serve state
{
    serve_attempts = 0;
    has_hit_table = false;
    has_hit_net = false;
}

void run_serve_detection_phase()
{
    static uint8_t serve_attempts = 0;
    static bool has_hit_table = false;
    static bool has_hit_net = false;
    absolute_time_t current_time = get_absolute_time();
    uint8_t opposing_player = (State.player_serving == PLAYER_1) ? PLAYER_2 : PLAYER_1;

    uint16_t result;
    bool isBounce[3] = {false, false, false};
    uint8_t threshold;

    for (size_t i = 0; i < 3; i++)
    {
        if (absolute_time_diff_us(State.last_read_time[i], current_time) > BOUNCE_SAMPLING_RATE_MS * 1000)
        {
            result = Piezos[i].read();
            threshold = (i == NET) ? SENSITIVITY_THRESHOLD_NET : SENSITIVITY_THRESHOLD_TABLE;
            isBounce[i] = (result > State.piezo_dc_biases[i] + threshold || result < State.piezo_dc_biases[i] - threshold);
            if (isBounce[i])
                State.last_read_time[i] = current_time;
        }
    }

    // Ignores "bounces" if they do not occur where the ball is positioned - rejects accidental contacts
    if (State.ball_location != PLAYER_1 && State.rpi_connected)
        isBounce[PLAYER_1] = false;
    if (State.ball_location != PLAYER_2 && State.rpi_connected)
        isBounce[PLAYER_2] = false;
    if (!State.ball_is_center && State.rpi_connected)
        isBounce[NET] = false;

    // Ignores net bounce if occurs just after table bounce (due to net clamps the net piezos pick up the vibration too)
    if (absolute_time_diff_us(State.prev_bounce_time, current_time) < 100000)
        isBounce[NET] = false;

    if (isBounce[PLAYER_1] || isBounce[PLAYER_2])
    {
        isBounce[NET] = false;
        State.prev_bounce_time = current_time;
    }
    if (isBounce[PLAYER_1])
        printf("Player 1 side hit\r\n");
    else if (isBounce[PLAYER_2])
        printf("Player 2 side hit\r\n");
    else if (isBounce[NET])
        printf("Net hit\r\n");
    else if (has_hit_table && absolute_time_diff_us(State.prev_bounce_time, current_time) > TIME_OUT_THRESHOLD_MS * 1000) // If served but went out (doesn't matter if it hit the net or not)
    {
        printf("Served but went out\r\n");
        score_point(opposing_player);
        reset_serve_state(serve_attempts, has_hit_table, has_hit_net);
    }
    else
        return;

    if (isBounce[State.player_serving]) // Detects hitting server side table
    {
        printf("Serving side %d hit\r\n", State.player_serving + 1);
        has_hit_table = true;
        State.prev_bounce_time = current_time;
        clear_all_leds(); // Turns off led that was indicating serve side
    }

    if (has_hit_table && isBounce[NET]) // Detects hitting the net
    {
        printf("Net hit after serve\r\n");
        has_hit_net = true;
    }

    if (has_hit_table && has_hit_net && isBounce[opposing_player]) // Detects a let
    {
        printf("Let detected\r\n");
        serve_attempts++;
        display_word("let."); // Display word 'let' on display
        sleep_ms(3000);       // Wait a moment before moving on and changing back to score

        if (serve_attempts > State.settings.num_lets_allowed && State.settings.limited_lets) // If too many lets in a row are served -> point for opposing player
        {
            printf("Too many lets in a row: %d\r\n", serve_attempts);
            score_point(opposing_player);
            serve_attempts = 0;
        }
        else
            State.phase = SETUP_ROUND;
        display_player_score(State.player_score[0], State.player_score[1]);

        has_hit_table = false;
        has_hit_net = false;
    }

    else if (has_hit_table && has_hit_net && isBounce[State.player_serving]) // Hits server side, then hits net, then falls back onto server side -> point for opposing player
    {
        printf("Point to Player %d, hit net fall back\r\n", opposing_player + 1);
        score_point(opposing_player);
        reset_serve_state(serve_attempts, has_hit_table, has_hit_net);
    }

    else if (has_hit_table && !has_hit_net && isBounce[opposing_player]) // Hits server side, then opposing player side without touching net -> Serve successful
    {
        printf("Serve successful\r\n");
        reset_serve_state(serve_attempts, has_hit_table, has_hit_net);
        State.prev_bounce_side = opposing_player;
        State.prev_bounce_time = current_time;
        State.phase = BOUNCE_LISTEN;
    }

    // State.camera_check_return_to_mode = SERVE_DETECTION;
    // State.phase = CAMERA_CHECK;
}