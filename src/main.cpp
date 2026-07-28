#include "board.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"

// Drivers
#include "drivers/logging/logging.h"
#include "drivers/piezos/piezos.h"
#include "drivers/display/display.h"

// Global Constants
#define SENSITIVITY_THRESHOLD 80

// Global Variables ================================================================================================================
Piezo Piezo1(VIBRATION_OUTPUT1_PIN, BUZZER1_PIN);
Piezo Piezo2(VIBRATION_OUTPUT2_PIN, BUZZER2_PIN);
Piezo Piezo3(27, 15);
uint8_t mode = PIEZO_TEST_MODE;
bool mode_change_logged = false;

// Define functions for mode cycling================================================================================================
void on_board_button_callback(uint gpio, uint32_t events)
{
    mode = (mode < NUM_MODES - 1) ? mode + 1 : DEFAULT_MODE;
    mode_change_logged = false;
}

/**
 * @brief This function allows for mode changes during sleeping
 * @param ms, milliseconds to sleep for (will round down to multiple of 10)
 * @param expected_mode expected mode when called
 * @return Returns true if mode changed, false otherwise
 */
bool sleep_ms_with_checking(uint16_t ms, uint8_t expected_mode)
{
    for (size_t i = 0; i < (ms / 10); i++)
    {
        sleep_ms(10);
        if (mode != expected_mode)
            return true;
    }
    return false;
}

// Define mode functions ==========================================================================================================

void run_default_mode()
{
    printf("Default mode running\r\n"); // placeholder
    if (sleep_ms_with_checking(5000, DEFAULT_MODE))
        return;                                      // placeholder
    printf("This can be stopped from printing\r\n"); // placeholder
    return;
}

void run_piezo_test_mode()
{
    static uint8_t bounces = 0;
    static uint8_t side = PLAYER_1_SIDE;

    if (!Piezo1.buzzer_on || !Piezo2.buzzer_on)
    {
        uint16_t result1 = Piezo1.read();
        uint16_t result2 = Piezo2.read();
        uint16_t result3 = Piezo3.read();
        // printf("%d,%d\r\n", result1, result2);

        if (result1 > 2020 + SENSITIVITY_THRESHOLD || result1 < 2020 - SENSITIVITY_THRESHOLD)
        {
            if (side == PLAYER_1_SIDE)
            {
                bounces++;
                Piezo1.play_victory_sequence();
            }
            else
            {
                bounces = 1;
                side = PLAYER_1_SIDE;
            }
            printf("BOUNCE1: %d\r\n", bounces);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        if (result2 > 2020 + SENSITIVITY_THRESHOLD || result2 < 2020 - SENSITIVITY_THRESHOLD)
        {
            if (side == PLAYER_2_SIDE)
            {
                bounces++;
                Piezo2.play_victory_sequence();
            }
            else
            {
                bounces = 1;
                side = PLAYER_2_SIDE;
            }
            printf("BOUNCE2: %d\r\n", bounces);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        // printf("%d\r\n", result3);
        if (result3 > 3234 + SENSITIVITY_THRESHOLD || result3 < 3234 - SENSITIVITY_THRESHOLD)
        {

            printf("BOUNCE3: %d\r\n", bounces++);
            Piezo3.play_victory_sequence();
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }
    }

    return;
}

void test_display()
{
    display_number(1234);
    sleep_ms(200);
    clear_display();
    sleep_ms(200);
    display_number(1234);
    sleep_ms(200);
    blank_digits(1, 2);
    sleep_ms(200);
}

// Main ===========================================================================================================================

int main()
{
    stdio_init_all();
    Piezo1.init_sensing();
    Piezo2.init_sensing();
    set_up_display(SDA_MOSI_PIN, SCL_SCLK_PIN);
    gpio_init(ON_BOARD_SW_PIN);
    gpio_set_dir(ON_BOARD_SW_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ON_BOARD_SW_PIN, GPIO_IRQ_EDGE_RISE, true, &on_board_button_callback);
    clear_display();
    while (true)
    {
        switch (mode)
        {
        case DEFAULT_MODE:
            if (!mode_change_logged)
            {
                log(INFORMATION, "Mode Changed: Mode = Default Mode");
                mode_change_logged = true;
            }
            run_default_mode();
            break;

        case PIEZO_TEST_MODE:
            if (!mode_change_logged)
            {
                log(INFORMATION, "Mode Changed: Mode = Test Piezo Mode");
                mode_change_logged = true;
                Piezo2.play_victory_sequence();
            }
            run_piezo_test_mode();
            break;
        case DISPLAY_TEST_MODE:
            if (!mode_change_logged)
            {
                log(INFORMATION, "Mode Changed: Mode = Test Display Mode");
                mode_change_logged = true;
            }
            test_display();
            break;
        }
    }

    return 0;
}
