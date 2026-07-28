#include "display.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

static uint8_t display_buffer[9] = {0};

void set_up_display(uint8_t sda_pin, uint8_t scl_pin)
{
    i2c_init(I2C_PORT, 400 * 1000); // Set the clock (400kHz)
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    // gpio_pull_up(sda_pin);
    // gpio_pull_up(scl_pin); // May not need these... only if forgot resistors??
    uint8_t cmd;
    cmd = OSCILLATOR_ON;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);
    cmd = DISPLAY_ON;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);
    cmd = BRIGHTNESS;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);
}

void display_number(int value)
{
    display_buffer[0] = 0;
    static const uint16_t divisors[] = {1000, 100, 10, 1};
    for (int digit = 0; digit < 4; digit++)
    {
        uint8_t segments = digits[(value / divisors[digit]) % 10];
        display_buffer[1 + digit * 2] = segments;
        display_buffer[2 + digit * 2] = 0;
    }
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, display_buffer, sizeof(display_buffer), false);
}

void clear_display()
{
    uint8_t tx[9] = {0};
    // tx[0] = 0 is the display RAM address
    // the remaining 8 bytes are all zero, so all segments are off
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false);
}

void blank_digits(uint8_t first_digit, uint8_t last_digit)
{
    // Clamp to valid range 1-4
    if (first_digit < 1)
        first_digit = 1;
    if (last_digit > 4)
        last_digit = 4;
    // Swap if backwards
    if (first_digit > last_digit)
    {
        uint8_t temp = first_digit;
        first_digit = last_digit;
        last_digit = temp;
    }
    for (uint8_t digit = first_digit; digit <= last_digit; digit++)
    {
        // Digit 1 = leftmost, digit 4 = rightmost
        display_buffer[1 + (digit - 1) * 2] = 0;
    }
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, display_buffer, sizeof(display_buffer), false);
}