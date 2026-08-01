#include <stdio.h>
#include "pico/stdlib.h"
#include "WS2812.pio.h" // This header file gets produced during compilation from the WS2812.pio file

#include "leds.h"
#include "board.h"
#include "helpers/colours/colours.h"
#include "drivers/logging/logging.h"

static uint32_t leds_set_data[MAX_NUM_LED];     // Colours to be set to LEDs
static uint32_t leds_current_data[MAX_NUM_LED]; // Colours currently on LEDs
static bool is_leds_updated;

void init_leds()
{
    uint pio_program_offset = pio_add_program(LED_PIO, &ws2812_program);
    ws2812_program_init(LED_PIO, LED_SM, pio_program_offset, EXTERNAL_LED_IN_PIN, 800000, false);
    clear_all_leds();
}

void set_single_led(uint8_t led_index, rgb_colour colour)
{
    // Check for invalid index (start index = 0)
    if (led_index >= MAX_NUM_LED)
    {
        log(LogLevel::ERROR, "Incorrect LED index: index < total number of leds");
        return;
    }
    leds_set_data[led_index] = pack_colour(colour);
    is_leds_updated = false;
}

void set_all_leds(rgb_colour colour)
{
    for (uint i = 0; i < MAX_NUM_LED; i++)
    {
        leds_set_data[i] = pack_colour(colour);
    }
    is_leds_updated = false;
}

void clear_all_leds()
{
    set_all_leds(get_rgb(OFF));
    update_all_leds();
}

void print_status_leds()
{
    if (is_leds_updated == false)
    {
        log(LogLevel::INFORMATION, RED_TEXT "Colours not updated " DEFAULT_TEXT "printing values...");
        printf("------------------------------------------------------------\n");
        for (uint i = 0; i < MAX_NUM_LED; i++)
        {
            // Unpack each colour
            uint set_r = (leds_set_data[i] >> 24) & 0xFF;
            uint set_g = (leds_set_data[i] >> 16) & 0xFF;
            uint set_b = (leds_set_data[i] >> 8) & 0xFF;

            uint current_r = (leds_current_data[i] >> 24) & 0xFF;
            uint current_g = (leds_current_data[i] >> 16) & 0xFF;
            uint current_b = (leds_current_data[i] >> 8) & 0xFF;

            const char *set_colour_index = get_colour_name(set_r, set_g, set_b);
            const char *current_colour_index = get_colour_name(current_r, current_g, current_b);

            // If not updated, print red
            if (set_r != current_r || set_g != current_g || set_b != current_b)
            {
                printf(RED_TEXT "LED %2u: Set %-9s || Current %-9s" DEFAULT_TEXT "\n", i, set_colour_index, current_colour_index);
            }
            else
            {
                printf("LED %2u: Set %-9s || Current %-9s\n", i, set_colour_index, current_colour_index);
            }
        }
        printf("------------------------------------------------------------\n");
    }
    else
    {
        log(LogLevel::INFORMATION, GREEN_TEXT "Colours up to date" DEFAULT_TEXT);
    }
}

void update_all_leds()
{
    for (uint i = 0; i < MAX_NUM_LED; i++)
    {
        pio_sm_put_blocking(LED_PIO, LED_SM, leds_set_data[i]);
        leds_current_data[i] = leds_set_data[i];
    }

    sleep_us(540);
    is_leds_updated = true;
}