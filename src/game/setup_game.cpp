#include "setup_game.h"

void set_game_mode(GameMode mode)
{
    State.game_mode = mode;
    State.settings = get_game_mode_settings(State.game_mode);
}

void run_setup_game_mode()
{
    // Determine game mode
    GameMode detected_mode = hat_id_read_mode();
    if (detected_mode != State.game_mode)
    {
        set_game_mode(detected_mode);
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

    State.mode = SETUP_ROUND;
    return;
}