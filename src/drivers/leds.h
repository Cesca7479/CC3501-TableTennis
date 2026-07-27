#pragma once

#include <cstdint>
#include "pico/stdlib.h"
#include "WS2812.pio.h"

class LEDDriver
{
private:
    uint32_t *led_data;
    uint8_t num_leds;
    bool is_sent;

public:
    LEDDriver(uint8_t leds);
    ~LEDDriver();

    void send_data();
    void turn_off();
    bool has_pending_changes();

    void set_single(uint8_t chosen_led, uint8_t red, uint8_t green, uint8_t blue);
    void set_all(uint8_t red, uint8_t green, uint8_t blue);
    void set_range(uint8_t start_led, uint8_t end_led, uint8_t red, uint8_t green, uint8_t blue);
    void set_multiple(uint32_t *led_values, uint8_t length);
    void set_number_of_leds(uint8_t leds);
    void set_single_hsv(uint8_t chosen_led, uint8_t hue, uint8_t saturation, uint8_t value);
    void set_all_hsv(uint8_t hue, uint8_t saturation, uint8_t value);
    void set_range_hsv(uint8_t start_led, uint8_t end_led, uint8_t hue, uint8_t saturation, uint8_t value);
    void hsv_to_rgb(uint8_t hue, uint8_t saturation, uint8_t value, uint8_t &red, uint8_t &green, uint8_t &blue);
    void set_multiple_hsv(uint8_t *hue_values, uint8_t *saturation_values, uint8_t *value_values, uint8_t length);

    uint32_t *get_led_data();
};
