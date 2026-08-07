#include "display.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include <cctype>

static uint8_t display_buffer[9] = {0};

// LEt
// nA.
// CAS.
// Pro.S
// Pro.L
// No.So

struct Letters
{
    // 0b0GFEDCBA
    uint8_t a = 0b01110111;
    uint8_t b = 0b01111100;
    // uint8_t c = 0b01011000; this one is lowercase
    uint8_t c = 0b00111001; // uppercase C
    uint8_t d = 0b01011110;
    uint8_t e = 0b01111001;
    uint8_t f = 0b01110001;
    uint8_t g = 0b00111101;
    uint8_t h = 0b01110100;
    uint8_t i = 0b00000110;
    uint8_t j = 0b00011110;
    uint8_t l = 0b00111000;
    uint8_t n = 0b01010100;
    uint8_t o = 0b01011100;
    uint8_t p = 0b01110011;
    uint8_t q = 0b01100111;
    uint8_t r = 0b01010000;
    uint8_t s = 0b01101101;
    uint8_t t = 0b01111000;
    uint8_t u = 0b00111110;
    uint8_t y = 0b01101110;
    uint8_t z = 0b01011011;
} letters;

uint8_t letter_to_segments(char c)
{
    switch (std::tolower(static_cast<unsigned char>(c)))
    {
    case 'a':
        return letters.a;
    case 'b':
        return letters.b;
    case 'c':
        return letters.c;
    case 'd':
        return letters.d;
    case 'e':
        return letters.e;
    case 'f':
        return letters.f;
    case 'g':
        return letters.g;
    case 'h':
        return letters.h;
    case 'i':
        return letters.i;
    case 'j':
        return letters.j;
    case 'l':
        return letters.l;
    case 'n':
        return letters.n;
    case 'o':
        return letters.o;
    case 'p':
        return letters.p;
    case 'q':
        return letters.q;
    case 'r':
        return letters.r;
    case 's':
        return letters.s;
    case 't':
        return letters.t;
    case 'u':
        return letters.u;
    case 'y':
        return letters.y;
    case 'z':
        return letters.z;
    default:
        return 0; // blank for unsupported characters
    }
}

void display_word(const char *word)
{
    display_buffer[0] = 0;
    uint8_t display_segments[4] = {0, 0, 0, 0};
    int digit = 0;
    for (int i = 0; word[i] != '\0' && digit < 4; i++)
    {
        // Decimal point applies to the previous digit
        if (word[i] == '.')
        {
            if (digit > 0)
            {
                display_segments[digit - 1] |= 0x80;
            }
            continue;
        }
        display_segments[digit++] = letter_to_segments(word[i]);
    }

    // Copy into HT16K33 buffer
    for (int d = 0; d < 4; d++)
    {
        display_buffer[1 + d * 2] = display_segments[d];
        display_buffer[2 + d * 2] = 0;
    }

    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, display_buffer, sizeof(display_buffer), false);
}

void display_init(uint8_t sda_pin, uint8_t scl_pin)
{
    i2c_init(I2C_PORT, 400 * 1000); // Set the clock (400kHz)
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin); // May not need these... only if forgot resistors??
    uint8_t cmd;
    cmd = OSCILLATOR_ON;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);
    cmd = DISPLAY_ON;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);
    cmd = BRIGHTNESS;
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, &cmd, 1, false);
}

void display_number(int value)
{
    display_buffer[0] = 0;
    static const uint16_t divisors[] = {1000, 100, 10, 1};
    for (int digit = 0; digit < 4; digit++)
    {
        uint8_t segments = digits[(value / divisors[digit]) % 10];
        display_buffer[1 + digit * 2] = segments;
        display_buffer[2 + digit * 2] = 0;
    }
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, display_buffer, sizeof(display_buffer), false);
}

void display_clear()
{
    uint8_t tx[9] = {0};
    // tx[0] = 0 is the display RAM address
    // the remaining 8 bytes are all zero, so all segments are off
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false);
}

void display_clear_digits(uint8_t first_digit, uint8_t last_digit)
{
    // Clamp to valid range 1-4
    if (first_digit < 1)
        first_digit = 1;
    if (last_digit > 4)
        last_digit = 4;
    // Swap if backwards
    if (first_digit > last_digit)
    {
        uint8_t temp = first_digit;
        first_digit = last_digit;
        last_digit = temp;
    }
    for (uint8_t digit = first_digit; digit <= last_digit; digit++)
    {
        // Digit 1 = leftmost, digit 4 = rightmost
        display_buffer[1 + (digit - 1) * 2] = 0;
    }
    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, display_buffer, sizeof(display_buffer), false);
}

void display_player_score(uint8_t player_1_score, uint8_t player_2_score)
{
    display_number(player_1_score * 100 + player_2_score);
}

void display_clear_individual_score(uint8_t player_number)
{
    if (player_number == 0)
    {
        display_clear_digits(1, 2);
    }
    else if (player_number == 1)
    {
        display_clear_digits(3, 4);
    }
}

void display_flat_lines()
{
    uint8_t tx[9] = {0};
    tx[0] = 0x00;
    for (uint8_t i = 1; i < 8; i += 2)
    {
        tx[i] = 0b01000000;
    }

    i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false);
}

void display_mode(GameMode mode)
{
    display_clear();
    switch (mode)
    {
    case GameMode::CASUAL:
        display_word("cas.");
        break;

    case GameMode::PROFESSIONAL_SHORT:
        display_word("pro.s");
        break;

    case GameMode::PROFESSIONAL_LONG:
        display_word("pro.l");
        break;

    case GameMode::NO_SOUND:
        display_word("no.so");
        break;

    default:
        display_word("none");
        break;
    }
}

void display_loading(uint wait_time)
{
    uint8_t tx[9] = {0};
    tx[0] = 0x00; // Is this necessary?
    uint8_t flat_line = 0b01000000;
    for (uint8_t i = 1; i < 8; i += 2)
    {
        tx[i] = flat_line;
        i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false);
        sleep_ms(wait_time);
    }
    // Optional to make it also 'load' off the screen
    // for (uint8_t i = 1; i < 8; i += 2)
    // {
    //     tx[i] = 0;
    //     i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false);
    //     sleep_ms(wait_time);
    // }

    // This is if the above is backwards
    // for (uint8_t i = 7; i > 0; i -= 2)
    // {
    //     tx[i] = flat_line;
    //     i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false);
    //     sleep_ms(wait_time);
    // }
    // for (uint8_t i = 7; i > 0; i -= 2)
    // {
    //     tx[i] = 0;
    //     i2c_write_blocking(I2C_PORT, HT16K33_ADDR, tx, sizeof(tx), false);
    //     sleep_ms(wait_time);
    // }
}