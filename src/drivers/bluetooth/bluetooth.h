#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"

#define BT_UART uart1 // Board-specific

void bluetooth_init(uint8_t tx_pin, uint8_t rx_pin, uint8_t reset_pin);
void reset_bluetooth();
void handle_bluetooth_message();
void clear_bluetooth_buffer();
void bluetooth_send(const char *message);