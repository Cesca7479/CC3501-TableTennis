#pragma once

#include "board.h"
#include "pico/stdlib.h"
#include "helpers/game_settings/game_settings.h"
#include "drivers/piezos/piezos.h"


// Mode definitions
#include "setup_round.h"
#include "setup_game.h"
#include "serve_detection.h"
#include "bounce_listen.h"
#include "check_victory_and_score.h"


#define SENSITIVITY_THRESHOLD_TABLE 80
#define SENSITIVITY_THRESHOLD_NET 80
#define BOUNCE_SAMPLING_RATE_MS 100 // Sample once every 100ms to prevent overcounting bounces
#define TIME_OUT_THRESHOLD_MS 2000

enum State
{
    SETUP_GAME,
    SETUP_ROUND,
    SERVE_DETECTION,
    BOUNCE_LISTEN,
    CAMERA_CHECK,
    CHECK_VICTORY_AND_SCORE,
    FOUL  
};


struct GameState
{
    uint8_t mode = SETUP_GAME;
    GameMode game_mode = GameMode::NO_MODE_SELECTED;
    uint8_t game_number = 0;
    uint8_t player_score[2] = {0, 0};
    uint8_t player_serving = PLAYER_1;
    uint8_t prev_bounce_side;
    absolute_time_t prev_bounce_time;
    absolute_time_t last_read_time[3] = {0,0,0};
    GameSettings settings = GameSettings{};
    uint16_t piezo_dc_biases[3];
};

extern GameState State;


