#include "testing.h"

#include "drivers/hat_id/hat_id.h"
#include "game/setup_game.h"
#include "drivers/user_buttons/user_buttons.h"

uint8_t mode = MUSIC_TEST_MODE;
uint8_t mode = USER_BUTTON_TEST_MODE;
bool mode_change_logged = false;

// Define functions for mode cycling================================================================================================
void on_board_button_callback(uint gpio, uint32_t events)
{
    mode = (mode < NUM_MODES - 1) ? mode + 1 : DEFAULT_MODE;
    mode_change_logged = false;
}

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
        if (result1 > sum_piezo1 / 10 + SENSITIVITY_THRESHOLD_TABLE || result1 < sum_piezo1 / 10 - SENSITIVITY_THRESHOLD_TABLE)
            if (result1 > sum_piezo1 / 10 + SENSITIVITY_THRESHOLD_TABLE || result1 < sum_piezo1 / 10 - SENSITIVITY_THRESHOLD_TABLE)
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
                printf("BOUNCE1: %d, Distance: %d\r\n", bounces, result1 - sum_piezo1 / 10);
                printf("BOUNCE1: %d, Distance: %d\r\n", bounces, result1 - sum_piezo1 / 10);
                if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                    return;
            }
        if (result1 > sum_piezo1 / 10 + SENSITIVITY_THRESHOLD_TABLE || result1 < sum_piezo1 / 10 - SENSITIVITY_THRESHOLD_TABLE)
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
            printf("BOUNCE1: %d, Distance: %d\r\n", bounces, result1 - sum_piezo1 / 10);
            printf("BOUNCE1: %d, Distance: %d\r\n", bounces, result1 - sum_piezo1 / 10);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        if (result2 > sum_piezo2 / 10 + SENSITIVITY_THRESHOLD_TABLE || result2 < sum_piezo2 / 10 - SENSITIVITY_THRESHOLD_TABLE)
            if (result2 > sum_piezo2 / 10 + SENSITIVITY_THRESHOLD_TABLE || result2 < sum_piezo2 / 10 - SENSITIVITY_THRESHOLD_TABLE)
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
                printf("BOUNCE2: %d, Distance: %d\r\n", bounces, result2 - sum_piezo2 / 10);
                printf("BOUNCE2: %d, Distance: %d\r\n", bounces, result2 - sum_piezo2 / 10);
                if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                    return;
            }
        if (result2 > sum_piezo2 / 10 + SENSITIVITY_THRESHOLD_TABLE || result2 < sum_piezo2 / 10 - SENSITIVITY_THRESHOLD_TABLE)
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
            printf("BOUNCE2: %d, Distance: %d\r\n", bounces, result2 - sum_piezo2 / 10);
            printf("BOUNCE2: %d, Distance: %d\r\n", bounces, result2 - sum_piezo2 / 10);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        if (result3 > sum_piezo3 / 10 + SENSITIVITY_THRESHOLD_NET || result3 < sum_piezo3 / 10 - SENSITIVITY_THRESHOLD_NET)
            if (result3 > sum_piezo3 / 10 + SENSITIVITY_THRESHOLD_NET || result3 < sum_piezo3 / 10 - SENSITIVITY_THRESHOLD_NET)
            {
                if (result3 > sum_piezo3 / 10 + SENSITIVITY_THRESHOLD_NET || result3 < sum_piezo3 / 10 - SENSITIVITY_THRESHOLD_NET)
                {

                    printf("BOUNCE3: %d, Distance: %d\r\n", bounces, result3 - sum_piezo3 / 10);
                    printf("BOUNCE3: %d, Distance: %d\r\n", bounces, result3 - sum_piezo3 / 10);
                    // Piezo3.play_victory_sequence();
                    if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                        return;
                }
                printf("BOUNCE3: %d, Distance: %d\r\n", bounces, result3 - sum_piezo3 / 10);
                printf("BOUNCE3: %d, Distance: %d\r\n", bounces, result3 - sum_piezo3 / 10);
                // Piezo3.play_victory_sequence();
                if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                    return;
            }
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
    uint8_t player_1_score = 20;
    uint8_t player_2_score = 19;
    display_player_score(player_1_score, player_2_score);
    sleep_ms(2000);
    display_clear_individual_score(1);
    sleep_ms(2000);
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
    motor_move_motor_safely(CENTRE);
    sleep_ms(1000);
    motor_move_motor_safely(LEFT);
    sleep_ms(2000);
}

void run_led_test_mode()
{
    for (;;)
    {
        for (uint i = 0; i < 7; i++)
        {
            set_single_led(0, get_rgb(rainbow[i]));
            update_all_leds();
            sleep_ms(1000);
        }
    }
}

void run_referee_test_mode()
{
    // printf("REFEREE ANGY!\n");
    // referee_angry(2000);
    // sleep_ms(1000);

    // Cha cha dance
    printf("REFEREE DANCE!\n");
    referee_dance(1000, 500);
    referee_dance(500, 250);

    // printf("REFEREE POINT LEFT\n");
    // referee_point_scored(LEFT);
    // sleep_ms(2000);
    // clear_all_leds();

    // printf("REFEREE POINT RIGHT\n");
    // referee_point_scored(RIGHT);
    // sleep_ms(2000);
    // clear_all_leds();

    // printf("REFEREE SERVE LEFT!\n");
    // referee_indicate_server(LEFT);
    // sleep_ms(2000);
    // clear_all_leds();

    // printf("REFEREE SERVE RIGHT!\n");
    // referee_indicate_server(RIGHT);
    // sleep_ms(2000);
    // clear_all_leds();
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
    printf("musssiiiiiic\n");
    Piezos[2].play_victory_sequence();
    sleep_ms(500);
    Piezos[2].play_angry_sounds();
    sleep_ms(500);
    Piezos[2].play_point();
    sleep_ms(500);
    Piezos[2].play_serve();
    sleep_ms(500);
    Piezos[2].play_select();
    sleep_ms(500);
}

void run_user_buttons_test_mode()
{
    user_buttons_update();
    for (uint8_t i = 0; i < 3; i++)
    {
        UserButton button = static_cast<UserButton>(i);

        if (is_button_pressed(button))
        {
            printf("%s pressed\n", user_button_to_string(button));
        }

        if (is_button_released(button))
        {
            printf("%s released\n", user_button_to_string(button));
        }
    }
}