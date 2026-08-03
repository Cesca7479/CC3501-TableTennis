#pragma once

#include "pico/stdlib.h"

enum class GameMode 
{
    NO_MODE_SELECTED,
    CASUAL,
    PROFESSIONAL_SHORT,
    PROFESSIONAL_LONG,
    NO_SOUND,
    UNKNOWN
};

enum Sound_Length
{
    NONE,
    SHORT,
    LONG
};


struct GameSettings
{
    uint8_t points_to_win;
    bool win_by_two;
    uint8_t sound_length;
    uint8_t serves_per_player;
    bool limited_lets;
    uint8_t num_lets_allowed;
};

/**
 * @brief Get the game settings of a game mode
 * 
 * @param mode The desired mode
 * @return A struct of the game settings for that mode
 */
GameSettings get_game_mode_settings(GameMode mode);
