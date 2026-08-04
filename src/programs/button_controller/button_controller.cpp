#include <stdio.h>

#include "pico/stdlib.h"

#include "button_controller.h"
#include "drivers/user_buttons/user_buttons.h"
#include "game/gamestate.h"

// void button_controller_update()
// {
//     // Get pressed states of all buttons
//     bool left_pressed = is_button_pressed(LEFT_BUTTON);
//     bool select_pressed = is_button_pressed(SELECT_BUTTON);
//     bool right_pressed = is_button_pressed(RIGHT_BUTTON);

//     // Get current game state?
//     switch (State.mode)
//     {
//     case SETUP_GAME:
//     // side switching
//     // SELECT_BUTTON -> clear final score, display 1 and 2 for player 1 and 2 on either side of display
//     // LEFT/RIGHT BUTTON -> switch side 1 and 2 are on (with animation)
//     // SELECT BUTTON -> display current mode, update when hat is changed
//     // LEFT/RIGHT BUTTON -> cycle through, last score (enable changes), side switching, current mode, start game
//     // SELECT BUTTON -> select chosen setting to change

//     // SELECT BUTTON (start game) -> start new game
//     case SETUP_ROUND:
//     // allow score to be changed
//     case SERVE_DETECTION:
//     // allow score to be changed
//     case BOUNCE_LISTEN:
//     // allow score to be changed
//     case CAMERA_CHECK:
//     // allow score to be changed
//     case CHECK_VICTORY_AND_SCORE:
//     // allow score to be changed
//     // on score change, revert to SETUP_ROUND
//     case FOUL:
//     //na
//     }
// }

/* allow score to be changed:
 SELECT_BUTTON -> change to CHANGE SCORE MODE
 LEFT/RIGHT BUTTON -> select side to change (blinking)
 SELECT BUTTON -> select side
 LEFT/RIGHT BUTTON -> increase/decrease score
 SELECT BUTTON -> confirm chosen score and exit CHANGE SCORE MODE
 go to CHECK VICTORY AND SCORE */