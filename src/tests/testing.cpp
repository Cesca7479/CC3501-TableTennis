#include "testing.h"

uint8_t mode = PIEZO_TEST_MODE;
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
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        // printf("result2: %d\r\n", result2);
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
            printf("BOUNCE2: %d, Distance: %d\r\n", bounces, result2 - sum_piezo2 / 10);
            if (sleep_ms_with_checking(100, PIEZO_TEST_MODE))
                return;
        }

        if (result3 > sum_piezo3 / 10 + SENSITIVITY_THRESHOLD_NET || result3 < sum_piezo3 / 10 - SENSITIVITY_THRESHOLD_NET)
        {

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
    printf("Motor test left right centre");
    move_motor_position_safely(CENTRE);
    sleep_ms(1000);
    move_motor_position_safely(LEFT);
    sleep_ms(2000);
    // printf("led test");
    // set_all_leds(get_rgb(GREEN));
    // update_all_leds();
    // sleep_ms(1000);
    // set_single_led(0, get_rgb(WHITE));
    // set_single_led(1, get_rgb(RED));
    // update_all_leds();
    // sleep_ms(1000);
}