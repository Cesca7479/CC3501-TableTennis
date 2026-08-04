#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "hat_id.h"
#include "board.h"
#include "helpers/game_settings/game_settings.h"
#include "drivers/display/display.h"

#define ADC_REF_VOLTAGE_MILLI 3300
#define ADC_INPUT 1
#define ADC_MAX_COUNTS 4096

void hat_id_init()
{
    adc_init();
    adc_gpio_init(HAT_ID_PIN);
}

GameMode hat_id_read_mode()
{
    adc_select_input(ADC_INPUT);
    uint32_t raw = adc_read();
    uint32_t voltage = (raw * ADC_REF_VOLTAGE_MILLI) / ADC_MAX_COUNTS;

    // R2 = NA voltage = 3.3V
    if (voltage > 2500)
    {
        display_word("none");
        return GameMode::NO_MODE_SELECTED;
    }
    // R2 = 10k voltage = 0.58V
    else if (voltage >= 300 && voltage < 800)
    {
        display_word("cas.");
        return GameMode::CASUAL;
    }
    // R2 = 22k voltage = 1.05V
    else if (voltage >= 800 && voltage < 1300)
    {
        display_word("pro.s");
        return GameMode::PROFESSIONAL_SHORT;
    }
    // R2 = 47k voltage = 1.65V
    else if (voltage >= 1300 && voltage < 1900)
    {
        display_word("pro.l");
        return GameMode::PROFESSIONAL_LONG;
    }
    // R2 = 100k voltage = 2.25V
    else if (voltage >= 1900 && voltage < 2500)
    {
        display_word("no.so");
        return GameMode::NO_SOUND;
    }

    return GameMode::UNKNOWN;
}
