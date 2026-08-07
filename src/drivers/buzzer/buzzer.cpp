#include "buzzer.h"

const uint32_t notes5[7] = {523, 587, 659, 698, 784, 879, 988};
const uint32_t notes6[7] = {1047, 1175, 1319, 1397, 1568, 1760, 1976};
const uint32_t notes7[7] = {2093, 2349, 2637, 2794, 3136, 3520, 3951};

uint slice_num;
uint8_t GPIO_buzzer_pin;

void buzzer_init(uint8_t GPIO_pin)
{
    GPIO_buzzer_pin = GPIO_pin;
    gpio_set_function(GPIO_buzzer_pin, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(GPIO_buzzer_pin);
    pwm_set_clkdiv(slice_num, 4.0f);
}

void buzzer_play_tone(uint32_t frequency)
{
    uint32_t wrap = 125000000 / (4 * frequency) - 1;
    pwm_set_wrap(slice_num, wrap);
    pwm_set_gpio_level(GPIO_buzzer_pin, wrap / 2);
    pwm_set_enabled(slice_num, true);
}

void buzzer_stop()
{
    pwm_set_enabled(slice_num, false);
}

void buzzer_play_victory_sequence()
{
    buzzer_play_tone(notes6[2]);
    sleep_ms(100);
    buzzer_play_tone(notes6[0]);
    sleep_ms(100);
    buzzer_play_tone(notes6[2]);
    sleep_ms(100);
    buzzer_play_tone(notes6[4]);
    sleep_ms(100);
    buzzer_play_tone(notes6[2]);
    sleep_ms(100);
    buzzer_play_tone(notes6[4]);
    sleep_ms(100);
    buzzer_play_tone(notes7[0]);
    sleep_ms(300);

    buzzer_stop();
}

void buzzer_play_angry_sounds()
{
    for (uint8_t i = 0; i < 20; i++)
    {
        buzzer_play_tone(notes7[0]);
        sleep_ms(20);
        buzzer_play_tone(notes6[6]);
        sleep_ms(20);
    }
    buzzer_stop();
}

void buzzer_play_point_scored()
{
    buzzer_play_tone(notes6[6]);
    sleep_ms(200);
    buzzer_play_tone(notes7[2]);
    sleep_ms(200);

    buzzer_stop();
}

void buzzer_play_serve()
{
    for (uint8_t i = 0; i < 3; i++)
    {
        buzzer_play_tone(notes6[i * 2]);
        sleep_ms(200);
    }
    buzzer_stop();
}

void buzzer_play_select_pressed()
{
    for (uint8_t i = 0; i < 2; i++)
    {
        buzzer_play_tone(notes6[2]);
        sleep_ms(50);
        buzzer_play_tone(notes7[0]);
        sleep_ms(50);
    }
    buzzer_stop();
}