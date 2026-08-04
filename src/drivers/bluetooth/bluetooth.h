#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "game/gamestate.h"

#define BT_UART uart1 // Board-specific

void bluetooth_init(uint8_t tx_pin, uint8_t rx_pin, uint8_t reset_pin);
void bluetooth_reset();
void bluetooth_handle_message();
void bluetooth_compare_message(char buffer[64]);
void bluetooth_clear_buffer();
void bluetooth_send(const char *message);