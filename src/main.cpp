#include "board.h"


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"

// Drivers
#include "drivers/logging/logging.h"


// Setup modes and checking ========================================================================================================

uint8_t mode = DEFAULT_MODE;
bool mode_change_logged = false;


void on_board_button_callback(uint gpio, uint32_t events) {
    mode = (mode < NUM_MODES - 1) ? mode + 1 : DEFAULT_MODE;
    mode_change_logged = false;
}


/**  
 * @brief This function allows for mode changes during sleeping
 * @param ms, milliseconds to sleep for (will round down to multiple of 10)
 * @param expected_mode expected mode when called
 * @return Returns true if mode changed, false otherwise
 */
bool sleep_ms_with_checking(uint16_t ms, uint8_t expected_mode) {
    for (size_t i = 0; i < (ms/10); i++)
    {
        sleep_ms(10);
        if (mode != expected_mode) return true;
    }
    return false;
}



// Define mode functions ==========================================================================================================

void run_default_mode() {
    printf("Default mode running\r\n"); // placeholder
    if (sleep_ms_with_checking(5000, DEFAULT_MODE)) return; // placeholder
    printf("This can be stopped from printing\r\n"); // placeholder
    return;
}

void run_piezo_test_mode() {
    printf("Piezo test running\r\n"); // placeholder
    sleep_ms(1000); // placeholder
    return;
}


// Main ===========================================================================================================================

int main()
{
    stdio_init_all();

    gpio_init(ON_BOARD_SW_PIN);
    gpio_set_dir(ON_BOARD_SW_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ON_BOARD_SW_PIN, GPIO_IRQ_EDGE_RISE, true, &on_board_button_callback);


    while (true) {
        switch (mode) {
            case DEFAULT_MODE:
                if (!mode_change_logged) {
                    log(INFORMATION, "Mode Changed: Mode = Default Mode");
                    mode_change_logged = true;
                }
                run_default_mode();
                break;

            case PIEZO_TEST_MODE:
                if (!mode_change_logged) {
                    log(INFORMATION, "Mode Changed: Mode = Test Piezo Mode");
                    mode_change_logged = true;
                }
                run_piezo_test_mode();
                break;
        }
    }

    return 0;
}
