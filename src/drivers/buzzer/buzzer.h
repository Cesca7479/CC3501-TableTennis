#pragma once

#include "game/gamestate.h"


/**
 * @brief init buzzer
 */
void buzzer_init(uint8_t GPIO_pin);

/**
 * @brief play tone from buzzer
 * @param frequency Play tone at frequency
 */
void buzzer_play_tone(uint32_t frequency);

/**
 * @brief stop buzzer
 */
void buzzer_stop();

/**
 * @brief play victory sequence
 */
void buzzer_play_victory_sequence();

/**
 * @brief Play angry sounds - 2 notes rapdily changing between themselves, one semitone apart
 */
void buzzer_play_angry_sounds();

/**
 * @brief Play point tune - short 2 note phrase, going upwards from the 5th to the 1st of a major scale
 */
void buzzer_play_point_scored();

/**
 * @brief Play serve tune - short 3 note phrase, playing the 1st, 3rd and 5th of a major scale
 */
void buzzer_play_serve();

/**
 * @brief Play select tune - quick 4 note phrase, repeated from the 3rd to the 1st in the octave above
 */
void buzzer_play_select_pressed();