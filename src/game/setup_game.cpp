#include "setup_game.h"
#include "drivers/hat_id/hat_id.h"
#include "drivers/user_buttons/user_buttons.h"
#include "helpers/dc_bias/dc_bias.h"

void set_game_mode(GameMode mode)
{
    State.game_mode = mode;
    State.settings = get_game_mode_settings(State.game_mode);
}

void run_setup_game_mode()
{
    display_loading(125); // Transition to new display mode
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

    set_dc_biases();

    display_loading(125); // Transition to new display mode
    
    State.player_score[PLAYER_1] = 0;
    State.player_score[PLAYER_2] = 0;
    display_player_score(State.player_score[0], State.player_score[1]);
    State.mode = SETUP_ROUND;
    return;
}