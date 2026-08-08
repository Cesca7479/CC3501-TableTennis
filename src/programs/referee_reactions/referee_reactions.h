#pragma once

#include "drivers/motor/motor.h"
#include "helpers/colours/colours.h"

/** 
 * @brief Referee reaction for indicating which side is serving
 * @param side_serving The side that is serving 
 */
void referee_indicate_server(uint8_t player);

/** 
 * @brief Referee reaction for when a point is scored
 * @param side_scored The side that scored the point (LEFT or RIGHT)
 */
void referee_point_scored(uint8_t player);

/**
 * @brief Pre-choreographed dance sequence for victory 
 */
void referee_dance_sequence();

/**
 * @brief Referee reaction for when a player's been BAD
 * @param duration_ms The duration of the angry reaction in milliseconds
 */
void referee_angry(uint duration_ms);