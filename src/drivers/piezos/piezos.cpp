#include "piezos.h"

Piezo Piezos[3] = {
    Piezo(VIBRATION_OUTPUT1_PIN, BUZZER1_PIN),
    Piezo(VIBRATION_OUTPUT2_PIN, BUZZER2_PIN),
    Piezo(VIBRATION_OUTPUT3_PIN, BUZZER3_PIN)};

const uint32_t notes5[7] = {523, 587, 659, 698, 784, 880, 988};
const uint32_t notes6[7] = {1047, 1175, 1319, 1397, 1568, 1760, 1976};
const uint32_t notes7[7] = {2093, 2349, 2637, 2794, 3136, 3520, 3951};

void Piezo::init_sensing()
{
    adc_init();
    adc_gpio_init(GPIO_sensing_pin);
}

uint16_t Piezo::read()
{
    adc_select_input(ADC_input);
    raw_result = adc_read();
    return raw_result;
}

void Piezo::init_buzzer()
{
    buzzer_on = true;
    gpio_set_function(GPIO_buzzer_pin, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(GPIO_buzzer_pin);
    pwm_set_clkdiv(slice_num, 4.0f);
}

void Piezo::play_tone(uint32_t frequency)
{
    uint32_t wrap = 125000000 / (4 * frequency) - 1;
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(GPIO_buzzer_pin, wrap / 2);
    pwm_set_enabled(slice_num, true);
}

void Piezo::stop_buzzer()
{
    pwm_set_enabled(slice_num, false);
    gpio_set_function(GPIO_buzzer_pin, GPIO_FUNC_SIO);
    gpio_set_dir(GPIO_buzzer_pin, GPIO_OUT);
    gpio_put(GPIO_buzzer_pin, 0);
    sleep_ms(1000);
    gpio_set_dir(GPIO_buzzer_pin, GPIO_IN);
    gpio_disable_pulls(GPIO_buzzer_pin);

    // sleep_ms(1000);
    // init_sensing();
    // sleep_ms(1000);
    buzzer_on = false;
}

void Piezo::play_victory_sequence()
{
    init_buzzer();

    for (size_t i = 0; i < 7; i++)
    {
        play_tone(notes5[i]);
        sleep_ms(50);
    }
    stop_buzzer();
}