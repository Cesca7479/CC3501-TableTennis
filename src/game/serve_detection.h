#pragma once

#include "gamestate.h"

#include "pico/stdlib.h"
#include "stdio.h"
#include "drivers/display/display.h"

void reset_serve_state(uint8_t &serve_attempts, bool &has_hit_table, bool &has_hit_net);

void run_serve_detection_mode();