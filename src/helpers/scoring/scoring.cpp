#include "scoring.h"

#include "programs/referee_reactions/referee_reactions.h"
#include "game/gamestate.h"


void score_point(uint8_t player) {
    State.player_score[player]++;
    referee_point_scored(player);
    State.phase = CHECK_VICTORY_AND_SCORE;
}