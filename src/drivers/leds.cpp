#include "leds.h"
#include "board.h"

LEDDriver::LEDDriver(uint8_t leds)
{
    num_leds = leds;
    is_sent = true;
    led_data = new uint32_t[leds](); // Create an array to hold the LED data
}

LEDDriver::~LEDDriver()
{
    delete[] led_data;
}

bool LEDDriver::has_pending_changes()
{
    return !is_sent;
}

void LEDDriver::send_data()
{
    sleep_us(520); // Ensure reset time has been met before sending new data (280us reset time for WS2812, but 520us was required)
    for (uint8_t i = 0; i < num_leds; i++)
    {
        pio_sm_put_blocking(pio0, 0, led_data[i]);
    }
    is_sent = true;
}

void LEDDriver::set_number_of_leds(uint8_t leds)
{
    turn_off(); // Ensure all LEDs are turned off before changing configuration
    if (leds != num_leds)
    {
        delete[] led_data; // Free the old array
        num_leds = leds;
        led_data = new uint32_t[leds](); // Create a new array with the updated size
    }
}

void LEDDriver::set_single(uint8_t chosen_led, uint8_t red, uint8_t green, uint8_t blue)
{
    if (chosen_led < num_leds)
    {
        led_data[chosen_led] = (red << 24) | (green << 16) | (blue << 8);
        is_sent = false;
    }
}

void LEDDriver::set_all(uint8_t red, uint8_t green, uint8_t blue)
{
    for (uint8_t i = 0; i < num_leds; i++)
    {
        set_single(i, red, green, blue);
    }
}

void LEDDriver::set_range(uint8_t start_led, uint8_t end_led, uint8_t red, uint8_t green, uint8_t blue)
{
    if (start_led < num_leds && end_led < num_leds && start_led <= end_led)
    {
        for (uint8_t i = start_led; i <= end_led; i++)
        {
            set_single(i, red, green, blue);
        }
    }
}

void LEDDriver::set_multiple(uint32_t *led_values, uint8_t length)
{
    for (uint8_t i = 0; i < length && i < num_leds; i++)
    {
        led_data[i] = led_values[i];
    }
    is_sent = false;
}

void LEDDriver::turn_off()
{
    set_all(0, 0, 0);
    send_data();
}

uint32_t *LEDDriver::get_led_data()
{
    return led_data;
}

void LEDDriver::hsv_to_rgb(uint8_t hue, uint8_t saturation, uint8_t value, uint8_t &red, uint8_t &green, uint8_t &blue)
{
    if (saturation == 0)
    {
        red = green = blue = value;
        return;
    }

    uint8_t region = hue / 43;                     // 0-5
    uint8_t remainder = (hue - (region * 43)) * 6; // 0-255

    uint8_t p = (value * (255 - saturation)) / 255;
    uint8_t q = (value * (255 - ((saturation * remainder) / 255))) / 255;
    uint8_t t = (value * (255 - ((saturation * (255 - remainder)) / 255))) / 255;

    switch (region)
    {
    case 0:
        red = value;
        green = t;
        blue = p;
        break;
    case 1:
        red = q;
        green = value;
        blue = p;
        break;
    case 2:
        red = p;
        green = value;
        blue = t;
        break;
    case 3:
        red = p;
        green = q;
        blue = value;
        break;
    case 4:
        red = t;
        green = p;
        blue = value;
        break;
    default:
        red = value;
        green = p;
        blue = q;
        break;
    }
}

void LEDDriver::set_single_hsv(uint8_t chosen_led, uint8_t hue, uint8_t saturation, uint8_t value)
{
    uint8_t red, green, blue;
    hsv_to_rgb(hue, saturation, value, red, green, blue);
    set_single(chosen_led, red, green, blue);
}

void LEDDriver::set_all_hsv(uint8_t hue, uint8_t saturation, uint8_t value)
{
    for (uint8_t i = 0; i < num_leds; i++)
    {
        set_single_hsv(i, hue, saturation, value);
    }
}

void LEDDriver::set_range_hsv(uint8_t start_led, uint8_t end_led, uint8_t hue, uint8_t saturation, uint8_t value)
{
    if (start_led < num_leds && end_led < num_leds && start_led <= end_led)
    {
        for (uint8_t i = start_led; i <= end_led; i++)
        {
            set_single_hsv(i, hue, saturation, value);
        }
    }
}

void LEDDriver::set_multiple_hsv(uint8_t *hue_values, uint8_t *saturation_values, uint8_t *value_values, uint8_t length)
{
    for (uint8_t i = 0; i < length && i < num_leds; i++)
    {
        set_single_hsv(i, hue_values[i], saturation_values[i], value_values[i]);
    }
}