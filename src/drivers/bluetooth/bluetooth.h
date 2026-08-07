#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "game/gamestate.h"

#define BT_UART uart1 // Board-specific

/**
 * @brief Initialise the bluetooth communication
 * @param tx_pin RP2040 TX pin
 * @param rx_pin RP2040 RX pin
 * @param reset_pin Reset pin, must be active low
 */
void bluetooth_init(uint8_t tx_pin, uint8_t rx_pin, uint8_t reset_pin);

/**
 * @brief Reset the bluetooth connection
 */
void bluetooth_reset();

/**
 * @brief Receive a message from the connected bluetooth device
 */
void bluetooth_handle_message();

/**
 * @brief Compare received message with desired messages, adjusts variables accordingly
 * @param buffer The message to be compared as a list of characters
 */
void bluetooth_compare_message(char buffer[64]);

/**
 * @brief Clear buffer in the bluetooth module, in case of large build up of irrelevant messages
 */
void bluetooth_clear_buffer();

/**
 * @brief Sends message to the connected bluetooth module
 * @param message Message to be sent
 */
void bluetooth_send(const char *message);