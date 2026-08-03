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
#define SENSITIVITY_THRESHOLD_TABLE 80
#define SENSITIVITY_THRESHOLD_NET 80
#define BOUNCE_SAMPLING_RATE_MS 100 // Sample once every 100ms to prevent overcounting bounces

// Global Variables ================================================================================================================
bool Testing = false;
uint8_t mode = PIEZO_TEST_MODE;

Piezo Piezos[3] = {
    Piezo(VIBRATION_OUTPUT1_PIN, BUZZER1_PIN),
    Piezo(VIBRATION_OUTPUT2_PIN, BUZZER2_PIN),
    Piezo(VIBRATION_OUTPUT3_PIN, BUZZER3_PIN)
};


bool mode_change_logged = false;

struct Settings {
    uint8_t points_to_win = 11;
    bool win_by_two = true;
    uint8_t sound_length = SHORT;
    uint8_t serves_per_player = 2;
    bool limited_lets = true;
    uint8_t num_lets_allowed = 1;
    uint16_t time_out_threshold_ms = 5000;
};

struct GameState {
    uint8_t mode = SETUP_GAME;
    uint8_t game_number = 0;
    uint8_t player_score[2] = {0, 0};
    uint8_t player_serving = PLAYER_1;
    uint8_t prev_bounce_side;
    absolute_time_t prev_bounce_time;
    absolute_time_t last_read_time[3] = {0,0,0};
    Settings settings;
    uint16_t piezo_dc_biases[3];
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
    
    for (size_t i = 0; i < 3; i++)
    {
        Piezos[i].init_sensing();
    }

    gpio_init(ON_BOARD_SW_PIN);
    gpio_set_dir(ON_BOARD_SW_PIN, GPIO_IN);
    gpio_set_irq_enabled_with_callback(ON_BOARD_SW_PIN, GPIO_IRQ_EDGE_RISE, true, &on_board_button_callback);
    bluetooth_init(BT_UART_TX_PIN, BT_UART_RX_PIN, BT_RESET_PIN);
    set_up_display(SDA_MOSI_PIN, SCL_SCLK_PIN);
    display_number(0000);

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

    uint32_t sum_piezo1 = 0;
    uint32_t sum_piezo2 = 0;
    uint32_t sum_piezo3 = 0;

    uint16_t result1;
    uint16_t result2;
    uint16_t result3;

    for (size_t i = 0; i < 10; i++)
    {
        result1 = Piezos[1].read();
        result2 = Piezos[2].read();
        result3 = Piezos[3].read();

        sum_piezo1 += result1;
        sum_piezo2 += result2;
        sum_piezo3 += result3;
    }

    if (!Piezos[0].buzzer_on || !Piezos[1].buzzer_on)
    {
        result1 = Piezos[1].read();
        result2 = Piezos[2].read();
        result3 = Piezos[3].read();

        // printf("%d:%d:%d\r\n", result1, result2,result3);
        if (result1 > sum_piezo1/10 + SENSITIVITY_THRESHOLD_TABLE || result1 < sum_piezo1/10 - SENSITIVITY_THRESHOLD_TABLE)
        {
            if (side == PLAYER_1)
            {
                bounces++;
                // Piezo1.play_victory_sequence();
            }
            else
            {
                bounces = 1;
                side = PLAYER_1;
            }
            printf("BOUNCE1: %d, Distance: %d\r\n", bounces, result1 - sum_piezo1/10);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        if (result2 > sum_piezo2/10 + SENSITIVITY_THRESHOLD_TABLE || result2 < sum_piezo2/10 - SENSITIVITY_THRESHOLD_TABLE)
        {
            if (side == PLAYER_2)
            {
                bounces++;
                // Piezo2.play_victory_sequence();
            }
            else
            {
                bounces = 1;
                side = PLAYER_2;
            }
            printf("BOUNCE2: %d, Distance: %d\r\n", bounces, result2 - sum_piezo2/10);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        if (result3 > sum_piezo3/10 + SENSITIVITY_THRESHOLD_NET || result3 < sum_piezo3/10 - SENSITIVITY_THRESHOLD_NET)
        {

            printf("BOUNCE3: %d, Distance: %d\r\n", bounces, result3 - sum_piezo3/10);
            // Piezo3.play_victory_sequence();
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

void run_motor_test_mode()
{
    move_motor_position_safely(LEFT);
    move_motor_position_safely(RIGHT);
}

// Define GAME mode functions
void run_setup_game_mode()
{
    // KEYA PUT YOUR HATS IN HERE - this will be run every loop

    // Should: read from ADC -> determine what hat is in (consider setting a default mode if no hat is on so it doesnt break)
    // Change settings in GameState State.settings
    // On middle pushbutton press -> do the raise arm stuff & change state.mode to BOUNCE_LISTEN

    uint32_t sum_piezos[3] = {0,0,0};

    uint16_t result;

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            result = Piezos[j].read();
            sum_piezos[j] += result;
        }
        sleep_ms(100);
    }
    
    for (size_t i = 0; i < 3; i++)
    {
        State.piezo_dc_biases[i] = sum_piezos[i]/10;
    }

    printf("DC Biases: %d, %d, %d\r\n", State.piezo_dc_biases[0],State.piezo_dc_biases[1],State.piezo_dc_biases[2]);
    State.mode = SETUP_ROUND;
    return;
}


void run_setup_round_mode() 
{
    uint8_t total_points = State.player_score[PLAYER_1] + State.player_score[PLAYER_2];
    State.player_serving = (State.game_number + total_points / State.settings.serves_per_player) % 2; // PLAYER_1 = 0, PLAYER_2 = 1
    printf("Player %d serving\r\n", State.player_serving + 1);
    // raise arm and light and whatever
    State.mode = SERVE_DETECTION;
}

void reset_serve_state(uint8_t &serve_attempts, bool &has_hit_table, bool &has_hit_net) // Resets serve state
{
    serve_attempts = 0;
    has_hit_table = false;
    has_hit_net = false;
}

void run_serve_detection_mode()
{
    static uint8_t serve_attempts = 0;
    static bool has_hit_table = false;
    static bool has_hit_net = false;
    absolute_time_t current_time = get_absolute_time();
    uint8_t opposing_player = (State.player_serving == PLAYER_1) ? PLAYER_2 : PLAYER_1;

    uint16_t result;
    bool isBounce[3] = {false,false,false};
    uint8_t threshold;

    for (size_t i = 0; i < 3; i++)
    {
        if (absolute_time_diff_us(State.last_read_time[i], current_time) > BOUNCE_SAMPLING_RATE_MS * 1000) {
            result = Piezos[i].read();
            threshold = (i == NET) ? SENSITIVITY_THRESHOLD_NET : SENSITIVITY_THRESHOLD_TABLE;
            isBounce[i] = result > State.piezo_dc_biases[i] + threshold || result < State.piezo_dc_biases[i] - threshold;
            if (isBounce[i]) State.last_read_time[i] = current_time;
        }
    }

    if (isBounce[PLAYER_1] || isBounce[PLAYER_2]) isBounce[NET] = false;

    
    if (isBounce[PLAYER_1]) printf("Player 1 side hit\r\n");
    else if (isBounce[PLAYER_2]) printf("Player 2 side hit\r\n");
    else if (isBounce[NET]) printf("Net hit\r\n");
    else if (has_hit_table && absolute_time_diff_us(State.prev_bounce_time, current_time) > State.settings.time_out_threshold_ms * 1000) // If served but went out (doesn't matter if it hit the net or not)
    {
        printf("Served but went out\r\n");
        State.player_score[opposing_player]++;
        reset_serve_state(serve_attempts, has_hit_table, has_hit_net);
        State.mode = CHECK_VICTORY_AND_SCORE;
    }
    else return;


    if (isBounce[State.player_serving]) // Detects hitting server side table
    {
        printf("Serving side %d hit\r\n", State.player_serving + 1);
        has_hit_table = true; 
        State.prev_bounce_time = current_time;
    }
    
    if (has_hit_table && isBounce[NET]) // Detects hitting the net
    {
        printf("Net hit after serve\r\n");
        has_hit_net = true; 
    }


    if (has_hit_table && has_hit_net && isBounce[opposing_player]) // Detects a let
    {
        printf("Let detected\r\n");
        serve_attempts++;

        
        if (serve_attempts > State.settings.num_lets_allowed && State.settings.limited_lets) // If too many lets in a row are served -> point for opposing player
        {
            printf("Too many lets in a row: %d\r\n", serve_attempts);
            State.player_score[opposing_player]++;
            State.mode = CHECK_VICTORY_AND_SCORE;
            serve_attempts = 0;
        }
        else State.mode = SETUP_ROUND;

        has_hit_table = false;
        has_hit_net = false;
    }

    else if (has_hit_table && has_hit_net && isBounce[State.player_serving]) // Hits server side, then hits net, then falls back onto server side -> point for opposing player
    { 
        printf("Point to Player %d, hit net fall back\r\n", opposing_player + 1);
        State.player_score[opposing_player]++;
        reset_serve_state(serve_attempts, has_hit_table, has_hit_net);
        State.mode = CHECK_VICTORY_AND_SCORE;
    } 

    

    else if (has_hit_table && !has_hit_net && isBounce[opposing_player]) // Hits server side, then opposing player side without touching net -> Serve successful
    {
        printf("Serve successful\r\n");
        reset_serve_state(serve_attempts, has_hit_table, has_hit_net);
        State.prev_bounce_side = opposing_player;
        State.prev_bounce_time = current_time;
        State.mode = BOUNCE_LISTEN;
    }
}

void run_bounce_listening_mode()
{
    absolute_time_t current_time = get_absolute_time();

    uint16_t result;
    bool isBounce[2] = {false, false};

    for (size_t i = 0; i < 2; i++)
    {
        if (absolute_time_diff_us(State.last_read_time[i], current_time) > BOUNCE_SAMPLING_RATE_MS * 1000) {
            result = Piezos[i].read();
            isBounce[i] = result > State.piezo_dc_biases[i] + SENSITIVITY_THRESHOLD_TABLE || result < State.piezo_dc_biases[i] - SENSITIVITY_THRESHOLD_TABLE;
            if (isBounce[i]) State.last_read_time[i] = current_time;
        }
    }

    if (isBounce[PLAYER_1] && State.prev_bounce_side == PLAYER_1) // Detects double bounce in player 1 side
    {
        printf("Double bounce in Player 1 side\r\n");
        State.player_score[PLAYER_2]++;
        State.mode = CHECK_VICTORY_AND_SCORE;
    }
    else if (isBounce[PLAYER_2] && State.prev_bounce_side == PLAYER_2) // Detects double bounce in player 2 side
    {
        printf("Double bounce in Player 2 side\r\n");
        State.player_score[PLAYER_1]++;
        State.mode = CHECK_VICTORY_AND_SCORE;
    }
    else if (isBounce[PLAYER_1]) // Sets previous bounce side and time
    {
        printf("Bounce in Player 1 side\r\n");
        State.prev_bounce_side = PLAYER_1;
        State.prev_bounce_time = current_time;
    }
    else if (isBounce[PLAYER_2]) // Sets previous bounce side and time
    {
        printf("Bounce in player 2 side\r\n");
        State.prev_bounce_side = PLAYER_2;
        State.prev_bounce_time = current_time;
    }
    else if (absolute_time_diff_us(State.prev_bounce_time, current_time) >= State.settings.time_out_threshold_ms * 1000) // Detects ball gone out by time threshold
    {
        printf("Ball went out\r\n");
        State.player_score[(State.prev_bounce_side == PLAYER_1) ? PLAYER_2 : PLAYER_1]++;
        State.mode = CHECK_VICTORY_AND_SCORE;
    }        
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

void run_check_victory_and_score_mode()
{
    display_number(State.player_score[PLAYER_1] * 100 + State.player_score[PLAYER_2]); // DISPLAY POINTS
    bool is_win = false;
    uint8_t winner;
    if (State.player_score[PLAYER_1] >= State.settings.points_to_win && (State.player_score[PLAYER_1] - 2 >= State.player_score[PLAYER_2] || !State.settings.win_by_two)) {
        is_win = true;
        winner = PLAYER_1;
    }
    else if (State.player_score[PLAYER_2] >= State.settings.points_to_win && (State.player_score[PLAYER_2] - 2 >= State.player_score[PLAYER_1] || !State.settings.win_by_two)) {
        is_win = true;
        winner = PLAYER_2;
    }
    else {
        State.mode = SETUP_ROUND;
    }

    if (is_win) {
        printf("Player %d has won!\r\n", winner + 1);
        State.mode = SETUP_GAME;
    }
    // referee_dance();
    return;
}

void run_foul_mode() {
    // referee_angry();
    return;
}
// Main ===========================================================================================================================

int main()
{
    init_board();
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
            case SETUP_GAME:
                run_setup_game_mode();
                break;
            case SETUP_ROUND:
                run_setup_round_mode();
                break;
            case SERVE_DETECTION:
                run_serve_detection_mode();
                break;
            case BOUNCE_LISTEN:
                run_bounce_listening_mode();
                break;
            case CAMERA_CHECK:
                run_camera_check_mode();
                break;
            case CHECK_VICTORY_AND_SCORE:
                run_check_victory_and_score_mode();
                break;
            case FOUL:
                run_foul_mode();
                break;
            }
        }
    }
    return 0;
}
