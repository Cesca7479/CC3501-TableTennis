#include "setup_game.h"
#include "drivers/hat_id/hat_id.h"
#include "drivers/user_buttons/user_buttons.h"

void set_game_mode(GameMode mode)
{
    State.game_mode = mode;
    State.settings = get_game_mode_settings(State.game_mode);
}

void run_setup_game_mode()
{
    display_mode(State.game_mode); // Retain previous game mode on display
    // Loop until game mode is confirmed
    while (!is_button_pressed(SELECT_BUTTON))
    {
        wait_for_select_release();
        GameMode detected_mode = hat_id_read_mode();
        if (detected_mode != State.game_mode)
        {
            set_game_mode(detected_mode);
            display_mode(detected_mode);
        }
    }

    // Determine piezo DC biases
    uint32_t sum_piezos[3] = {0, 0, 0};
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
        State.piezo_dc_biases[i] = sum_piezos[i] / 10;
    }

    printf("DC Biases: %d, %d, %d\r\n", State.piezo_dc_biases[0], State.piezo_dc_biases[1], State.piezo_dc_biases[2]);

    State.player_score[PLAYER_1] = 0;
    State.player_score[PLAYER_2] = 0;
    display_player_score(State.player_score[0], State.player_score[1]);
    State.mode = SETUP_ROUND;
    return;
}