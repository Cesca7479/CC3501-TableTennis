#include "piezos.h"

Piezo Piezos[3] = {
    Piezo(VIBRATION_OUTPUT1_PIN, BUZZER1_PIN),
    Piezo(VIBRATION_OUTPUT2_PIN, BUZZER2_PIN),
    Piezo(VIBRATION_OUTPUT3_PIN, BUZZER3_PIN)};

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
    play_tone(notes6[5]);
    sleep_ms(800);
    play_tone(notes6[3]);
    sleep_ms(400);
    play_tone(notes6[4]);
    sleep_ms(400);
    play_tone(notes6[5]);
    sleep_ms(400);
    play_tone(notes6[4]);
    sleep_ms(200);
    play_tone(notes6[5]);
    sleep_ms(600);

    // for (size_t i = 0; i < 7; i++)
    // {
    //     play_tone(notes5[i]);
    //     sleep_ms(500);
    // }
    stop_buzzer();
}

void Piezo::play_angry_sounds()
{
    init_buzzer();
    for (uint8_t i = 0; i < 20; i++)
    {
        play_tone(notes7[0]);
        sleep_ms(20);
        play_tone(notes6[6]);
        sleep_ms(20);
    }
    stop_buzzer();
}

void Piezo::play_point()
{
    init_buzzer();
    play_tone(notes6[6]);
    sleep_ms(200);
    play_tone(notes7[2]);
    sleep_ms(200);
    stop_buzzer();
}

void Piezo::play_serve()
{
    init_buzzer();
    for (uint8_t i = 0; i < 3; i++)
    {
        play_tone(notes6[i * 2]);
        sleep_ms(200);
    }
    stop_buzzer();
}

void Piezo::play_select()
{
    init_buzzer();
    for (uint8_t i = 0; i < 2; i++)
    {
        play_tone(notes6[2]);
        sleep_ms(50);
        play_tone(notes7[0]);
        sleep_ms(50);
    }
    stop_buzzer();
}