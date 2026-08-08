#pragma once

#include "pico/stdlib.h"
#include "board.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "drivers/logging/logging.h"
#include "stdio.h"

class Piezo
{

private:
    int ADC_input;
    int GPIO_sensing_pin;

public:
    void init();
    uint16_t read();
    Piezo(int sensing_pin)
    {
        GPIO_sensing_pin = sensing_pin;
        ADC_input = GPIO_sensing_pin - 26;
    };
};



extern Piezo Piezos[3];
