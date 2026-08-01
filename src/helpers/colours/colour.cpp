#include <stdio.h>

#include "pico/stdlib.h"
#include "colours.h"

rgb_colour get_rgb(LedColour colour)
{
    return colour_info_table[colour].rgb;
}

uint32_t pack_colour(rgb_colour colour)
{
    return ((uint32_t)colour.r << 24) | ((uint32_t)colour.g << 16) | ((uint32_t)colour.b << 8);
}

const char *get_colour_name(uint8_t r, uint8_t g, uint8_t b)
{
    for (uint i = 0; i < NUM_COLOURS; i++)
    {
        const rgb_colour rgb = colour_info_table[i].rgb;

        if (rgb.r == r &&
            rgb.g == g &&
            rgb.b == b)
        {
            return colour_info_table[i].name;
        }
    }

    return "CUSTOM";
}