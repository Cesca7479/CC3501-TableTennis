#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"

#include "WS2812.pio.h"
#include "drivers/logging/logging.h"

#define LED_PIN 14

// For the led display
#define I2C_PORT i2c0
#define SCL_PIN 17
#define SDA_PIN 16
#define HT16K33_ADDR 0x70
#define OSCILLATOR_ON 0x21
#define DISPLAY_ON 0x81
#define BRIGHTNESS 255
const uint8_t digits[] =
    {
        0b00111111, // 0
        0b00000110, // 1
        0b01011011, // 2
        0b01001111, // 3
        0b01100110, // 4
        0b01101101, // 5
        0b01111101, // 6
        0b00000111, // 7
        0b01111111, // 8
        0b01101111  // 9
};

void ht16k33_display_number(int value, uint8_t decimal_mask = 0)
{
    uint8_t tx[9];
    tx[0] = 0;
    static const uint16_t divisors[] = {1000, 100, 10, 1};

    for (int digit = 0; digit < 4; digit++)
    {
        uint8_t segments = digits[(value / divisors[digit]) % 10];

        // Turn on decimal point if requested
        if (decimal_mask & (1 << (3 - digit)))
        {
            segments |= 0x80;
        }
        tx[1 + digit * 2] = segments;
        tx[2 + digit * 2] = 0;
    }

    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false); // sizeof(tx) = 9 for all lit up. Change this later if don't want
}

int main()
{
    stdio_init_all();

    // Initialise PIO0 to control the LED chain
    uint pio_program_offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, pio_program_offset, LED_PIN, 800000, false);
    uint32_t led_data[1];

    // Set up led display
    i2c_init(I2C_PORT, 400 * 1000); // Set the clock (400kHz)
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN); // May not need these... only if forgot resistors??

    uint8_t cmd;

    cmd = OSCILLATOR_ON;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);

    cmd = DISPLAY_ON;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);

    cmd = BRIGHTNESS;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);

    for (;;)
    {
        // Test the log system
        log(LogLevel::INFORMATION, "Hello world");
        for (uint16_t i = 0; i <= 9999; i++)
        {
            ht16k33_display_number(i, i % 16);
            sleep_ms(100);
        }
    }

    return 0;
}
