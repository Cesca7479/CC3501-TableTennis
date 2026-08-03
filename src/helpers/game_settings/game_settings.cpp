#include "game_settings.h"

GameSettings get_game_mode_settings(GameMode mode)
{
    switch (mode)
    {
    case GameMode::CASUAL:
        return GameSettings{11, false, LONG, 2, false, 0};

    case GameMode::PROFESSIONAL_SHORT:
        return GameSettings{11, true, SHORT, 2, true, 1};

    case GameMode::PROFESSIONAL_LONG:
        return GameSettings{21, true, SHORT, 5, true, 1};

    case GameMode::NO_SOUND:
        return GameSettings{11, true, NONE, 2, true, 1};

    // Default to CASUAL mode on errors
    case GameMode::NO_MODE_SELECTED:
    case GameMode::UNKNOWN:
    default:
        return GameSettings{11, false, LONG, 2, false, 0};
    }
}