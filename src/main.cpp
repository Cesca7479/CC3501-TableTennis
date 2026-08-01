#include "board.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"

#include "WS2812.pio.h"

// Drivers =========================================================================================================================
#include "drivers/logging/logging.h"
#include "drivers/piezos/piezos.h"
#include "drivers/display/display.h"
#include "drivers/bluetooth/bluetooth.h"
#include "drivers/motor/motor.h"
#include "drivers/leds/leds.h"

#include "programs/referee_reactions/referee_reactions.h"

// Global Constants ================================================================================================================
#define SENSITIVITY_THRESHOLD 80

// Global Variables ================================================================================================================
bool Testing = true;

Piezo Piezo1(VIBRATION_OUTPUT1_PIN, BUZZER1_PIN);
Piezo Piezo2(VIBRATION_OUTPUT2_PIN, BUZZER2_PIN);
Piezo Piezo3(26, 15);
uint8_t mode = MOTOR_TEST_MODE;
bool mode_change_logged = false;

class GameState
{
public:
    uint8_t mode = SETUP;
    uint8_t player_score[2] = {0, 0};
    uint8_t player_serving = PLAYER_1;
    uint8_t points_to_win = 11;
    bool serve_successful = false;
    bool professional = false;
};

GameState State;

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

// Init board =====================================================================================================================
void init_board()
{
    stdio_init_all();
    Piezo1.init_sensing();
    Piezo2.init_sensing();

    gpio_init(ON_BOARD_SW_PIN);
    gpio_set_dir(ON_BOARD_SW_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ON_BOARD_SW_PIN, GPIO_IRQ_EDGE_RISE, true, &on_board_button_callback);
    bluetooth_init(BT_UART_TX_PIN, BT_UART_RX_PIN, BT_RESET_PIN);
    set_up_display(SDA_MOSI_PIN, SCL_SCLK_PIN);
    clear_display();

    init_motor_pwr_ctrl();
    init_motor();
}

// Define TESTING mode functions ==========================================================================================================

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
    static uint8_t side = PLAYER_1;

    if (!Piezo1.buzzer_on || !Piezo2.buzzer_on)
    {
        uint16_t result1 = Piezo1.read();
        uint16_t result2 = Piezo2.read();
        uint16_t result3 = Piezo3.read();

        if (result1 > 2020 + SENSITIVITY_THRESHOLD || result1 < 2020 - SENSITIVITY_THRESHOLD)
        {
            if (side == PLAYER_1)
            {
                bounces++;
                Piezo1.play_victory_sequence();
            }
            else
            {
                bounces = 1;
                side = PLAYER_1;
            }
            printf("BOUNCE1: %d\r\n", bounces);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        if (result2 > 2020 + SENSITIVITY_THRESHOLD || result2 < 2020 - SENSITIVITY_THRESHOLD)
        {
            if (side == PLAYER_2)
            {
                bounces++;
                Piezo2.play_victory_sequence();
            }
            else
            {
                bounces = 1;
                side = PLAYER_2;
            }
            printf("BOUNCE2: %d\r\n", bounces);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

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

void run_motor_test_mode() {
    move_motor_position_safely(LEFT);
    move_motor_position_safely(RIGHT);
}
// Define GAME mode functions
void run_setup_mode()
{
    // KEYA PUT YOUR HATS IN HERE - this will be run every loop

    // Should: read from ADC -> determine what hat is in (consider setting a default mode if no hat is on so it doesnt break)
    // Change settings in GameState State -> state. points_to_win, state.professional
    // Change state.serve_successful = false
    // On middle pushbutton press -> change state.mode to BOUNCE_LISTEN
    return;
}

void run_bounce_listening_mode()
{
    static absolute_time_t start = get_absolute_time();
    static absolute_time_t prev_bounce_time = get_absolute_time();
    static uint8_t prev_bounce_side;

    absolute_time_t current_time = get_absolute_time();

    if (absolute_time_diff_us(start, current_time) >= 100000) // Sample every 100ms to prevent overcounting bounces
    {
        uint16_t result1 = Piezo1.read();
        uint16_t result2 = Piezo2.read();
        uint16_t result3 = Piezo3.read();

        bool isBounce[3] = {result1 > 2020 + SENSITIVITY_THRESHOLD || result1 < 2020 - SENSITIVITY_THRESHOLD,
                            result2 > 2020 + SENSITIVITY_THRESHOLD || result2 < 2020 - SENSITIVITY_THRESHOLD,
                            result3 > 3234 + SENSITIVITY_THRESHOLD || result3 < 3234 - SENSITIVITY_THRESHOLD};

        if (!State.serve_successful)
        {
            static bool has_serve_bounced = false;
            if (isBounce[State.player_serving] && !has_serve_bounced)
                has_serve_bounced = true;
            else if (isBounce[State.player_serving] && has_serve_bounced)
                State.player_score[State.player_serving]--;
        }
        else
        {
            if (isBounce[PLAYER_1] && prev_bounce_side == PLAYER_1) // Detects double bounce in player 1 side
            {
                State.player_score[PLAYER_2]++;
                prev_bounce_side = PLAYER_1;
            }
            else if (isBounce[PLAYER_2] && prev_bounce_side == PLAYER_2) // Detects double bounce in player 2 side
            {
                State.player_score[PLAYER_1]++;
                prev_bounce_side = PLAYER_2;
            }
            else if (!isBounce[PLAYER_1] && !isBounce[PLAYER_2] && absolute_time_diff_us(prev_bounce_time, current_time) >= 2000000 && State.serve_successful) // Detects 2 second time elapsed since previous bounce
            {
                uint8_t winner = (prev_bounce_side == PLAYER_1) ? PLAYER_2 : PLAYER_1;
                State.player_score[winner]++;
            }
            display_number(State.player_score[PLAYER_1] * 100 + State.player_score[PLAYER_2]); // CHANGE WHEN DRIVER IS UPDATED
        }

        start = get_absolute_time();
    }

    return;
}

void run_camera_check_mode()
{
    return;
}

void run_point_add_mode()
{
    // referee_point_scored();
    return;
}

void run_victory_mode()
{
    // referee_dance();
    return;
}

// Main ===========================================================================================================================

int main()
{
    // init_board();
    stdio_init_all();
    init_motor_pwr_ctrl();
    init_motor();

    while (true)
    {
        if (Testing)
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
            case BLUETOOTH_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test Bluetooth Mode");
                    mode_change_logged = true;
                }
                handle_bluetooth_message();
                break;

            case MOTOR_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test Motor Mode");
                    mode_change_logged = true;
                }
                run_motor_test_mode();
                break;
            }
        }
        else
        {
            switch (State.mode)
            {
            case SETUP:
                run_setup_mode();
                break;
            case BOUNCE_LISTEN:
                run_bounce_listening_mode();
                break;
            case CAMERA_CHECK:
                run_camera_check_mode();
                break;
            case POINT_ADD:
                run_point_add_mode();
                break;
            case VICTORY:
                run_victory_mode();
                break;
            }
        }
    }
    return 0;
}
