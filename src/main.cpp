#include "board.h" //

#include <stdio.h>         //
#include "pico/stdlib.h"   //
#include "hardware/gpio.h" //
#include "hardware/i2c.h"  //
#include "hardware/pio.h"  //
#include <cstring>         //
#include <iterator>        //

#include "WS2812.pio.h" //

// Drivers =========================================================================================================================
#include "drivers/logging/logging.h" //
#include "drivers/piezos/piezos.h"   //
#include "drivers/display/display.h" //
#include "drivers/bluetooth/bluetooth.h"
#include "drivers/motor/motor.h" //
#include "drivers/leds/leds.h"
#include "drivers/hat_id/hat_id.h"             //
#include "drivers/user_buttons/user_buttons.h" //
#include "drivers/buzzer/buzzer.h"

// Helpers and programs ============================================================================================================
#include "programs/referee_reactions/referee_reactions.h" //
#include "helpers/game_settings/game_settings.h"          //
#include "helpers/dc_bias/dc_bias.h"

// Game Modes
#include "game/gamestate.h"

#include "game/setup_round.h"
#include "game/setup_game.h"
#include "game/serve_detection.h"
#include "game/bounce_listen.h"
#include "game/check_victory_and_score.h"
#include "game/camera_check.h"
#include "game/change_score.h"

// Testing Files
#include "tests/testing.h"

// Global Variables ================================================================================================================
bool Testing = false;
uint8_t mode = REFEREE_TEST_MODE;

// Init board =====================================================================================================================
void init_board()
{
    stdio_init_all();

    for (size_t i = 0; i < 3; i++)
    {
        Piezos[i].init();
    }
    buzzer_init(BUZZER_PIN);

    gpio_init(ON_BOARD_SW_PIN);
    gpio_set_dir(ON_BOARD_SW_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ON_BOARD_SW_PIN, GPIO_IRQ_EDGE_RISE, true, &on_board_button_callback);
    bluetooth_init(BT_UART_TX_PIN, BT_UART_RX_PIN, BT_RESET_PIN);

    // TODO: Update State.rpi_connected and State.camera_connected

    display_init(SDA_MOSI_PIN, SCL_SCLK_PIN);
    display_clear();
    user_buttons_init();

    init_leds();
    motor_pwr_ctrl_init();
    motor_init();
    hat_id_init();

    sleep_ms(500);
    set_dc_biases();

}

// Define GAME mode functions

void run_foul_mode() // not currently used - if used should be moved to external files
{
    // referee_angry();
    /* conditions for foul:
     - let + professional game
     - table slam (vibration detected but no camera agreement
     */
    return;
}
// Main ===========================================================================================================================

int main()
{
    init_board();
    set_game_mode(State.game_mode);

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
                    set_dc_biases();
                    // Piezo2.play_victory_sequence();
                }
                run_piezo_test_mode();
                break;
            case DISPLAY_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test Display Mode");
                    mode_change_logged = true;
                }
                // run_display_test_mode();
                // run_test_letters();
                // run_test_words();
                run_display_test_scores();
                break;
            case BLUETOOTH_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test Bluetooth Mode");
                    mode_change_logged = true;
                }
                bluetooth_handle_message();
                bluetooth_send("Player1: 53, Player2: 24\n");
                sleep_ms(2000);
                bluetooth_send("Won: Player2\n");
                sleep_ms(2000);
                break;

            case MOTOR_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test Motor Mode");
                    mode_change_logged = true;
                }
                run_motor_test_mode();
                break;

            case LED_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test LED Mode");
                    mode_change_logged = true;
                }
                run_led_test_mode();
                break;

            case REFEREE_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test referee Mode");
                    mode_change_logged = true;
                }
                run_referee_test_mode();
                break;

            case HAT_ID_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test hat ID Mode");
                    mode_change_logged = true;
                }
                run_hat_id_test_mode();
                break;

            case USER_BUTTON_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test user buttons Mode");
                    mode_change_logged = true;
                }
                run_user_buttons_test_mode();
                break;
            case MUSIC_TEST_MODE:
                if (!mode_change_logged)
                {
                    log(INFORMATION, "Mode Changed: Mode = Test music Mode");
                    mode_change_logged = true;
                }
                run_music_test_mode();
                break;
            }
        }
        else
        {
            bluetooth_handle_message();
            switch (State.phase)
            {
            case SETUP_GAME:
                run_setup_game_phase();
                break;
            case SETUP_ROUND:
                run_setup_round_phase();
                break;
            case SERVE_DETECTION:
                run_serve_detection_phase();
                break;
            case BOUNCE_LISTEN:
                run_bounce_listening_phase();
                break;
            case CAMERA_CHECK:
                run_camera_check_phase();
                break;
            case CHECK_VICTORY_AND_SCORE:
                run_check_victory_and_score_phase();
                break;
            case CHANGE_SCORE:
                run_change_score_phase();
                break;
                // case FOUL:
                //     run_foul_mode();
                //     break;

                // camera check and foul mode are not currently being used but can be added
            }
        }
    }
    return 0;
}
