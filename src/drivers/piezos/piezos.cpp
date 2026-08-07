#include "piezos.h"
#include "game/gamestate.h"

Piezo Piezos[3] = {
    Piezo(VIBRATION_OUTPUT1_PIN),
    Piezo(VIBRATION_OUTPUT2_PIN),
    Piezo(VIBRATION_OUTPUT3_PIN)};


void Piezo::init()
{
    adc_init();
    adc_gpio_init(GPIO_sensing_pin);
}

uint16_t Piezo::read()
{
    uint16_t raw_result;
    adc_select_input(ADC_input);
    raw_result = adc_read();
    return raw_result;
}