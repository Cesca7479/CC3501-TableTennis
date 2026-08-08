#include "testing.h"

#include "drivers/hat_id/hat_id.h"
#include "game/setup_game/setup_game.h"
#include "drivers/user_buttons/user_buttons.h"
#include "game/change_score/change_score.h"
#include "drivers/buzzer/buzzer.h"
#include "helpers/timing/timing.h"

bool mode_change_logged = false;

// Define functions for mode cycling================================================================================================
void on_board_button_callback(uint gpio, uint32_t events)
{
    mode = (mode < NUM_MODES - 1) ? mode + 1 : DEFAULT_MODE;
    mode_change_logged = false;
}

void run_default_mode()
{
    printf("Default mode running\r\n"); // placeholder
    if (sleep_ms_with_test_mode_checking(5000, DEFAULT_MODE))
        return;                                      // placeholder
    printf("This can be stopped from printing\r\n"); // placeholder
    return;
}

void run_piezo_test_mode()
{
    static uint8_t bounces = 0;
    static uint8_t side = PLAYER_1;
    static bool has_calculated_dc = false;
    uint32_t sum_piezo1 = 0;
    uint32_t sum_piezo2 = 0;
    uint32_t sum_piezo3 = 0;

    uint16_t result1;
    uint16_t result2;
    uint16_t result3;

    
    result1 = Piezos[0].read();
    result2 = Piezos[1].read();
    result3 = Piezos[2].read();

    // printf("%d:%d:%d\r\n", result1, result2,result3);
    if (result1 > State.piezo_dc_biases[0] + SENSITIVITY_THRESHOLD_TABLE || result1 < State.piezo_dc_biases[0] - SENSITIVITY_THRESHOLD_TABLE)
    {
        if (side == PLAYER_1)
        {
            bounces++;
        }
        else
        {
            bounces = 1;
            side = PLAYER_1;
        }
        printf("BOUNCE1: %d, Distance: %d\r\n", bounces, result1 - State.piezo_dc_biases[0]);
        sleep_ms(100);
    }

    if (result2 > State.piezo_dc_biases[1] + SENSITIVITY_THRESHOLD_TABLE || result2 < State.piezo_dc_biases[1] - SENSITIVITY_THRESHOLD_TABLE)
    {
        if (side == PLAYER_2)
        {
            bounces++;
        }
        else
        {
            bounces = 1;
            side = PLAYER_2;
        }
        printf("BOUNCE2: %d, Distance: %d\r\n", bounces, result2 - State.piezo_dc_biases[1]);
        sleep_ms(100);
    }

    if (result3 > State.piezo_dc_biases[2] + SENSITIVITY_THRESHOLD_TABLE || result3 < State.piezo_dc_biases[2] - SENSITIVITY_THRESHOLD_TABLE)
    {
        if (side == NET)
        {
            bounces++;
        }
        else
        {
            bounces = 1;
            side = NET;
        }
        printf("BOUNCE3: %d, Distance: %d\r\n", bounces, result3 - State.piezo_dc_biases[2]);
        sleep_ms(100);
    }

    return;
    
}

void run_display_test_mode()
{
    display_number(1234);
    sleep_ms(200);
    display_clear();
    sleep_ms(200);
    display_number(1234);
    sleep_ms(200);
    display_clear_digits(1, 2);
    sleep_ms(200);
}

void run_display_test_scores()
{
    // uint8_t player_1_score = 20;
    // uint8_t player_2_score = 19;
    // display_player_score(player_1_score, player_2_score);
    // sleep_ms(2000);
    // display_clear_individual_score(1);
    // sleep_ms(2000);
    display_loading(1000);
}

void run_test_letters()
{
    // All letters your display supports
    static const char supported[] = "abcdefghijlnopqrstuyz";

    const int length = strlen(supported);

    // Step through 4 letters at a time
    for (int i = 0; i < length; i += 4)
    {
        char word[5] = {' ', ' ', ' ', ' ', '\0'};

        for (int j = 0; j < 4; j++)
        {
            if (i + j < length)
            {
                word[j] = supported[i + j];
            }
        }

        display_word(word);
        sleep_ms(5000);
    }
}

void run_test_words()
{
    const char *words[] = {
        "let",
        "pro.L",
        "pro.S",
        "cas.",
        "no.so",
        "n.a."};
    for (uint8_t i = 0; i < std::size(words); i++)
    {
        display_word(words[i]);
        sleep_ms(2000);
    }
}

void run_motor_test_mode()
{
    printf("CENTRE\n");
    motor_move_motor_safely(CENTRE);
    sleep_ms(1000);
    printf("LEFT\n");
    motor_move_motor_safely(LEFT);
    sleep_ms(1000);
    printf("RIGHT\n");
    motor_move_motor_safely(RIGHT);
    sleep_ms(1000);
}

void run_led_test_mode()
{
    for (uint i = 0; i < 7; i++)
    {
        set_all_leds(get_rgb(rainbow[i]));
        update_all_leds();
        sleep_ms(50);
    }
}

void run_referee_test_mode()
{
    printf("REFEREE ANGY!\n");
    referee_angry(2000);
    sleep_ms(1000);

    // Cha cha dance
    printf("REFEREE DANCE!\n");
    referee_dance_sequence();
    sleep_ms(1000);

    printf("REFEREE POINT LEFT\n");
    referee_point_scored(LEFT);
    clear_all_leds();
    sleep_ms(1000);

    printf("REFEREE POINT RIGHT\n");
    referee_point_scored(RIGHT);
    clear_all_leds();
    sleep_ms(1000);

    printf("REFEREE SERVE LEFT!\n");
    referee_indicate_server(LEFT);
    clear_all_leds();
    sleep_ms(1000);

    printf("REFEREE SERVE RIGHT!\n");
    referee_indicate_server(RIGHT);
    clear_all_leds();
    sleep_ms(1000);
}

void run_change_score_test()
{
    run_change_score_phase();
}

void run_hat_id_test_mode()
{
    // Determine game mode
    GameMode detected_mode = hat_id_read_mode();
    if (detected_mode != State.game_mode)
    {
        set_game_mode(detected_mode);
    }
}

void run_music_test_mode()
{
    printf("victory\n");
    buzzer_play_victory_sequence();
    sleep_ms(2000);
    printf("angry\n");
    buzzer_play_angry_sounds();
    sleep_ms(2000);
    printf("point\n");
    buzzer_play_point_scored();
    sleep_ms(2000);
    printf("select\n");
    buzzer_play_select_pressed();
    sleep_ms(2000);
    printf("serve\n");
    buzzer_play_serve();
    sleep_ms(2000);
}

void run_user_buttons_test_mode()
{
    // user_buttons_update();
    // for (uint8_t i = 0; i < 3; i++)
    // {
    //     UserButton button = static_cast<UserButton>(i);

    //     if (is_button_pressed(button))
    //     {
    //         printf("%s pressed\n", user_button_to_string(button));
    //     }

    //     if (is_button_released(button))
    //     {
    //         printf("%s released\n", user_button_to_string(button));
    //     }
    // }
}