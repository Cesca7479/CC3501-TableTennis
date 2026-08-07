#include "piezos.h"
#include "game/gamestate.h"

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
    play_tone(notes6[2]);
    sleep_ms(100);
    play_tone(notes6[0]);
    sleep_ms(100);
    play_tone(notes6[2]);
    sleep_ms(100);
    play_tone(notes6[4]);
    sleep_ms(100);
    play_tone(notes6[2]);
    sleep_ms(100);
    play_tone(notes6[4]);
    sleep_ms(100);
    play_tone(notes7[0]);
    sleep_ms(300);

    // for (size_t i = 0; i < 7; i++)
    // {
    //     play_tone(notes5[i]);
    //     sleep_ms(500);
    // }
    stop_buzzer();
    sleep_ms(2000);
    // Determine piezo DC biases
    uint32_t sum_piezos[3] = {0, 0, 0};
    uint16_t result;

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            result = Piezos[j].read();
            sum_piezos[j] += result;
        }
        sleep_ms(100);
    }

    for (size_t i = 0; i < 3; i++)
    {
        State.piezo_dc_biases[i] = sum_piezos[i] / 10;
    }
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
        // Determine piezo DC biases
    uint32_t sum_piezos[3] = {0, 0, 0};
    uint16_t result;

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            result = Piezos[j].read();
            sum_piezos[j] += result;
        }
        sleep_ms(100);
    }

    for (size_t i = 0; i < 3; i++)
    {
        State.piezo_dc_biases[i] = sum_piezos[i] / 10;
    }
}

void Piezo::play_point()
{
    init_buzzer();
    play_tone(notes6[6]);
    sleep_ms(200);
    play_tone(notes7[2]);
    sleep_ms(200);
    stop_buzzer();
        // Determine piezo DC biases
    uint32_t sum_piezos[3] = {0, 0, 0};
    uint16_t result;

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            result = Piezos[j].read();
            sum_piezos[j] += result;
        }
        sleep_ms(100);
    }

    for (size_t i = 0; i < 3; i++)
    {
        State.piezo_dc_biases[i] = sum_piezos[i] / 10;
    }
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
        // Determine piezo DC biases
    uint32_t sum_piezos[3] = {0, 0, 0};
    uint16_t result;

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            result = Piezos[j].read();
            sum_piezos[j] += result;
        }
        sleep_ms(100);
    }

    for (size_t i = 0; i < 3; i++)
    {
        State.piezo_dc_biases[i] = sum_piezos[i] / 10;
    }
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
    sleep_ms(800);
    // Determine piezo DC biases
    uint32_t sum_piezos[3] = {0, 0, 0};
    uint16_t result;

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            result = Piezos[j].read();
            sum_piezos[j] += result;
        }
        sleep_ms(100);
    }

    for (size_t i = 0; i < 3; i++)
    {
        State.piezo_dc_biases[i] = sum_piezos[i] / 10;
    }
}