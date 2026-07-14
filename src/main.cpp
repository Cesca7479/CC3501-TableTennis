#include "board.h"
#include "drivers/leds.h"

uint8_t mode = RESET;

void on_board_button_callback(uint gpio, uint32_t events)
{
    mode = (mode < NUM_MODES - 1) ? mode + 1 : RESET;
}

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
    LEDDriver led_driver(NUM_LEDS);
    led_driver.turn_off(); // Ensure all LEDs are off at the start

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

    gpio_init(ON_BOARD_BUTTON);
    gpio_set_dir(ON_BOARD_BUTTON, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ON_BOARD_BUTTON, GPIO_IRQ_EDGE_RISE, true, &on_board_button_callback);

    gpio_init(LEFT_BUTTON);
    gpio_set_dir(LEFT_BUTTON, GPIO_IN);
    // gpio_set_irq_enabled_with_callback(LEFT_BUTTON, GPIO_IRQ_EDGE_RISE, true, &AAAAAAAAAAAAA);

    gpio_init(SELECT_BUTTON);
    gpio_set_dir(SELECT_BUTTON, GPIO_IN);
    // gpio_set_irq_enabled_with_callback(SELECT_BUTTON, GPIO_IRQ_EDGE_RISE, true, &AAAAAAAAAAA);

    gpio_init(RIGHT_BUTTON);
    gpio_set_dir(RIGHT_BUTTON, GPIO_IN);
    // gpio_set_irq_enabled_with_callback(RIGHT_BUTTON, GPIO_IRQ_EDGE_RISE, true, &AAAAAAAAAAA);

    uint8_t red;
    uint8_t green;
    uint8_t blue;

    for (;;)
    {
        // Test the log system
        log(LogLevel::INFORMATION, "Hello world");
        if (mode == RESET)
        {
            ht16k33_display_number(0);
        }
        uint16_t i = 0;
        while (mode == START)
        {
            ht16k33_display_number(i, 0b0100);
            sleep_ms(10);
            i++;
        }
        red = (gpio_get(LEFT_BUTTON)) ? 10 : 0;
        green = (gpio_get(SELECT_BUTTON)) ? 10 : 0;
        blue = (gpio_get(RIGHT_BUTTON)) ? 10 : 0;
        led_driver.set_all(red, green, blue);
        led_driver.send_data();
    }

    return 0;
}
