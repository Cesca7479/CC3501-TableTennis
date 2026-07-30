#include "drivers/bluetooth/bluetooth.h"
#include <cstring>
#include "hardware/uart.h"

uint8_t BT_RESET;

void bluetooth_init(uint8_t tx_pin, uint8_t rx_pin, uint8_t reset_pin)
{
    BT_RESET = reset_pin;
    uart_init(BT_UART, 115200);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    uart_set_format(BT_UART, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(BT_UART, true);
    gpio_init(reset_pin);
    gpio_set_dir(reset_pin, GPIO_OUT);
    gpio_put(reset_pin, 1);
}

void reset_bluetooth()
{
    gpio_put(BT_RESET, 0);
    printf("Bluetooth Disconnected\n");
    sleep_ms(500);
    gpio_put(BT_RESET, 1);
    printf("Ready to pair\n");
}

void handle_bluetooth_message()
{
    static char buffer[64];
    static uint8_t index = 0;

    while (uart_is_readable(BT_UART))
    {
        char c = uart_getc(BT_UART);
        // End of message
        if (c == '\n' || c == '\r')
        {
            buffer[index] = '\0';
            if (strcmp(buffer, "Reboot") == 0)
            {
                reset_bluetooth();
            }
            else if (strcmp(buffer, "Bounce") == 0)
            {
                printf("IT BOUNCED!!!!\n");
            }
            else if (strcmp(buffer, "PING") == 0)
            {
                printf("Bluetooth Connected\n");
                bluetooth_send("PONG\n");
            }
            // Reset for next message
            index = 0;
        }
        else if (index < sizeof(buffer) - 1)
        {
            buffer[index++] = c;
        }
    }
}

void clear_bluetooth_buffer()
{
    while (uart_is_readable(BT_UART))
    {
        volatile char dump = uart_getc(BT_UART);
    }
    sleep_ms(10);
    // Clear any messages that built up on the RN4870 buffer - may not be necessary?
    while (uart_is_readable(BT_UART))
    {
        volatile char dump = uart_getc(BT_UART);
    }
}

void bluetooth_send(const char *message)
{
    uart_puts(BT_UART, message);
}