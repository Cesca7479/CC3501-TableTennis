#pragma once

#include "gamestate.h"
#include "drivers/hat_id/hat_id.h"
#include "drivers/logging/logging.h"
#include "stdio.h"

/**
 * @brief Sets the current game mode
 * @param mode The mode to be set, if UNKNOWN or NO_MODE_SELECTED, defaults to CASUAL mode
 */
void set_game_mode(GameMode mode);

void run_setup_game_mode();