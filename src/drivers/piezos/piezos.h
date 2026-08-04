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
    uint16_t raw_result;
    int ADC_input;
    int num_bounces = 0;
    void adc_irq_handler();
    uint slice_num;
    int GPIO_sensing_pin;
    int GPIO_buzzer_pin;

public:
    bool buzzer_on = false;
    void init_sensing();
    void play_tone(uint32_t frequency);
    void play_victory_sequence();
    void play_angry_sounds();
    void play_point();
    void play_serve();
    void play_select();
    uint16_t read();
    void init_buzzer();
    void stop_buzzer();
    Piezo(int sensing_pin, int buzzer_pin)
    {
        GPIO_sensing_pin = sensing_pin;
        GPIO_buzzer_pin = buzzer_pin;
        ADC_input = GPIO_sensing_pin - 26;
    };
};

const uint32_t notes5[7] = {523, 587, 659, 698, 784, 879, 988};
const uint32_t notes6[7] = {1047, 1175, 1319, 1397, 1568, 1760, 1976};
const uint32_t notes7[7] = {2093, 2349, 2637, 2794, 3136, 3520, 3951};

extern Piezo Piezos[3];