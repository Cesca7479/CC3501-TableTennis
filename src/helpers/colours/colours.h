#pragma once

#include "pico/stdlib.h"

/// Stores a colour by seperate red, green and blue values of type uint8_t
struct rgb_colour
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/// Stores a colour by its colour name and rgb values
struct ColourInfo
{
    const char *name;
    rgb_colour rgb;
};

/// Stores a range of colours, where each colour is given an index that aligns with colour_info_table
enum LedColour
{
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    PURPLE,
    PINK,
    WHITE,
    OFF,
    NUM_COLOURS
};

/// Stores a range of colours, where each colour is stored by its name and its rgb values
static const ColourInfo colour_info_table[] =
    {
        {"RED", {100, 0, 0}},
        {"ORANGE", {100, 30, 0}},
        {"YELLOW", {100, 100, 0}},
        {"GREEN", {0, 100, 0}},
        {"BLUE", {0, 0, 100}},
        {"PURPLE", {50, 0, 100}},
        {"PINK", {100, 0, 50}},
        {"WHITE", {100, 100, 100}},
        {"OFF", {0, 0, 0}}};

static const LedColour rainbow[7] =
    {
        RED,
        ORANGE,
        YELLOW,
        GREEN,
        BLUE,
        PURPLE,
        PINK};

/**
 * @brief Get the rgb values of a given colour
 * @param colour Allows: RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, PINK, WHITE and OFF
 */
rgb_colour get_rgb(LedColour colour);

/// Pack a colour that contains rgb values into an appropriate format to send to LED chain
uint32_t pack_colour(rgb_colour colour);

/// Gets the colour name of a given rgb value, if it is not predefined, return 'CUSTOM'
const char *get_colour_name(uint8_t r, uint8_t g, uint8_t b);