#pragma once

#include "pico/stdlib.h"
#include "helpers/game_settings/game_settings.h"

/**
 * @brief Initialise hat ID
 */
void hat_id_init();

/**
 * @brief Reads game mode from hat
 */
GameMode hat_id_read_mode();

