#pragma once

#include "drivers/motor/motor.h"

/** 
 * @brief Flashes LEDs in a rainbow pattern
 * 
 * Note: move this function to an appropriate file in the future, as it is not specific to referee reactions
 * @param time_interval_ms The time interval between each color change in milliseconds
 */
void flash_leds_rainbow(uint time_interval_ms);

/** 
 * @brief Referee reaction for when a point is scored
 *
 * Note: change side positioning and arguments as required
 * @param side_scored The side that scored the point (LEFT or RIGHT)
 */
void referee_point_scored(ServoPosition side_scored);

/**
 * @brief Referee reaction for a dance 
 * 
 * Note: can be changed to victory dance, also is a blocking function which will need to be changed in future
 * @param duration_ms The duration of the dance in milliseconds
 */
void referee_dance(uint duration_ms);


/**
 * @brief Referee reaction for when a players been BAD
 * 
 * Note: is a blocking function which will need to be changed in future
 * @param bad_side The side that made the mistake (LEFT or RIGHT)
 * @param duration_ms The duration of the angry reaction in milliseconds
 */
void referee_angry(ServoPosition bad_side, uint duration_ms);