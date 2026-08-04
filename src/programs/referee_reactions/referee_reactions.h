#pragma once

#include "drivers/motor/motor.h"
#include "helpers/colours/colours.h"

/** 
 * @brief Flashes LEDs in a rainbow pattern
 * 
 * Note: move this function to an appropriate file in the future, as it is not specific to referee reactions
 * @param time_interval_ms The time interval between each color change in milliseconds
 */
void flash_leds_rainbow(uint time_interval_ms);

/** 
 * @brief Light led on side of specified player
 * @param player_side The side the specified player is on
 * @param colour The colour to light the led
 */
void light_player_side(ServoPosition player_side, rgb_colour colour);

/** 
 * @brief Referee reaction for indicating which side is serving
 * @param side_serving The side that is serving 
 */
void referee_indicate_server(ServoPosition side_serving);

/** 
 * @brief Referee reaction for when a point is scored
 * @param side_scored The side that scored the point (LEFT or RIGHT)
 */
void referee_point_scored(ServoPosition side_scored);

/**
 * @brief Perform one dance move, which moves arm once and changes colour 4 times
 * @param position Side to move motor to
 * @param duration_ms The duration of the dance in milliseconds
 * @param rainbow_index Reference to rainbow index
 * @param move_time Time for one dance move (time to move motor once)
 */
bool perform_dance_move(ServoPosition position, absolute_time_t duration_ms, uint8_t &rainbow_index, uint move_time);

/**
 * @brief Referee reaction for a dance 
 * @param duration_ms The duration of the dance in milliseconds
 */
void referee_dance(uint duration_ms, uint move_time);


/**
 * @brief Referee reaction for when a player's been BAD
 * @param duration_ms The duration of the angry reaction in milliseconds
 */
void referee_angry(uint duration_ms);