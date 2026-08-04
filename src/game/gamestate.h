#pragma once

#include "board.h"
#include "pico/stdlib.h"
#include "helpers/game_settings/game_settings.h"
#include "drivers/piezos/piezos.h"


#define SENSITIVITY_THRESHOLD_TABLE 80
#define SENSITIVITY_THRESHOLD_NET 80
#define BOUNCE_SAMPLING_RATE_MS 100 // Sample max once every 100ms per sensor to prevent overcounting bounces
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

enum Bounce_Location
{
    PLAYER_1,
    PLAYER_2,
    NET
};


struct GameState
{
    uint8_t mode = SETUP_GAME;
    uint8_t camera_check_return_to_mode = SERVE_DETECTION;
    GameMode game_mode = GameMode::NO_MODE_SELECTED;
    uint8_t game_number = 0;
    uint8_t player_score[2] = {0, 0};
    uint8_t player_serving = PLAYER_1;
    uint8_t prev_bounce_side;
    uint8_t ball_location = PLAYER_1;
    bool ball_is_center = false;
    absolute_time_t prev_bounce_time;
    absolute_time_t last_read_time[3] = {0,0,0};
    GameSettings settings = GameSettings{};
    uint16_t piezo_dc_biases[3];
};

extern GameState State;


