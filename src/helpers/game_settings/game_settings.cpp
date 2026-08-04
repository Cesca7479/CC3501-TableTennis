#include <stdio.h>

#include "game_settings.h"
#include "drivers/logging/logging.h"

GameSettings get_game_mode_settings(GameMode mode)
{
    switch (mode)
    {
    case GameMode::CASUAL:
        log(LogLevel::INFORMATION, "Game mode: CASUAL selected.");
        return GameSettings{11, false, LONG, 2, false, 0};

    case GameMode::PROFESSIONAL_SHORT:
        log(LogLevel::INFORMATION, "Game mode: PROFESSIONAL SHORT selected.");
        return GameSettings{11, true, SHORT, 2, true, 1};

    case GameMode::PROFESSIONAL_LONG:
        log(LogLevel::INFORMATION, "Game mode: PROFESSIONAL LONG selected.");
        return GameSettings{21, true, SHORT, 5, true, 1};

    case GameMode::NO_SOUND:
        log(LogLevel::INFORMATION, "Game mode: NO SOUND selected.");
        return GameSettings{11, true, NONE, 2, true, 1};

    // Default to CASUAL mode on errors
    case GameMode::NO_MODE_SELECTED:
        log(LogLevel::WARNING, "Game mode: NO MODE selected. Missing hat, defaulting to CASUAL mode.");
        return GameSettings{11, false, LONG, 2, false, 0};
    case GameMode::UNKNOWN:
        log(LogLevel::ERROR, "Game mode: UNKNOWN selected. Hat cannot be read, defaulting to CASUAL mode.");
        return GameSettings{11, false, LONG, 2, false, 0};
    }
}