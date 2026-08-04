#pragma once

#include "pico/stdlib.h"
#include "board.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "drivers/logging/logging.h"
#include "stdio.h"


class Piezo {

    private:
        uint16_t raw_result;
        int ADC_input;
        int num_bounces = 0;
        void adc_irq_handler();
        void init_buzzer();
        void stop_buzzer();
        uint slice_num;
        int GPIO_sensing_pin;
        int GPIO_buzzer_pin;
        
    public:
        bool buzzer_on = false;
        void init_sensing();
        void play_tone(uint32_t frequency);
        void play_victory_sequence();
        uint16_t read();
        Piezo(int sensing_pin, int buzzer_pin) {
            GPIO_sensing_pin = sensing_pin;
            GPIO_buzzer_pin = buzzer_pin;
            ADC_input = GPIO_sensing_pin - 26;
        };
};

extern Piezo Piezos[3];