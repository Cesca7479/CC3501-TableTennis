#include "pico/stdlib.h"


class Piezo {

    private:
        uint16_t raw_result;
        int ADC_input;
        int num_bounces = 0;
        void adc_irq_handler();
        void init_buzzer();
        void stop_buzzer();
        uint slice_num;
        
    public:
        bool buzzer_on = false;
        int GPIO_sensing_pin;
        int GPIO_buzzer_pin;
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