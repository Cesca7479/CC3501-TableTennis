#include "camera_check.h"

void run_camera_check_mode()
{
    // Cesca do your stuff to check bluetooth or call the function from your driver

    // if it returns that location has changed change:
    // State.ball_location = PLAYER_1 or PLAYER_2 (PLAYER_1 should be on the left facing our robot guy, left or right depends on where you want to setup the rpi)
    // State.ball_is_center = bool
    State.mode = State.camera_check_return_to_mode; // State changes back to either serve detection mode, or bounce listening mode
}