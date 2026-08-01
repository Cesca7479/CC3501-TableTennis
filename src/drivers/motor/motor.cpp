#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

#include "drivers/motor/motor.h"
#include "drivers/logging/logging.h"
#include "board.h"

/*
Each PWM period is 20 ms long (50 Hz)
The clock will be set to 1 MHz, so each count is 1 us. Allows for easier use with pulse widths
Use units of us as the PWM register uses an integer count and to simplify calculations
(floating-point calculations aren't supported on the RP2040)
*/

#define COUNTER_HZ 1000000  // How many times the PWM counter increments per second
#define PWM_PERIOD_US 20000 // 20ms

#define MOTOR_LEFT_PULSE_US 1500  
#define MOTOR_CENTRE_PULSE_US 1700 
#define MOTOR_RIGHT_PULSE_US 1900  

bool is_power_enabled = false;
bool is_motor_pwr_ctrl_initialised = false;

void init_motor_pwr_ctrl()
{
    gpio_init(MOTOR_PWR_CTRL_PIN);
    gpio_set_dir(MOTOR_PWR_CTRL_PIN, GPIO_OUT);
    gpio_put(MOTOR_PWR_CTRL_PIN, false); // Disable motor power on startup

    gpio_init(MOTOR_FAULT_FLAG_PIN);
    gpio_set_dir(MOTOR_FAULT_FLAG_PIN, GPIO_IN);
    is_power_enabled = false;
    is_motor_pwr_ctrl_initialised = true;
}

void init_motor()
{
    pwm_config config = pwm_get_default_config();
    int motor_pwm_slice = pwm_gpio_to_slice_num(TEST_MOTOR_IN_PIN);

    gpio_set_function(TEST_MOTOR_IN_PIN, GPIO_FUNC_PWM);

    // Slow down clock to COUNTER_HZ
    float clock_divider = CLK_SYS_HZ / COUNTER_HZ;
    pwm_config_set_clkdiv(&config, clock_divider);
    pwm_config_set_wrap(&config, PWM_PERIOD_US - 1);
    pwm_init(motor_pwm_slice, &config, false);

    pwm_set_gpio_level(TEST_MOTOR_IN_PIN, MOTOR_CENTRE_PULSE_US); // Default to centre position on startup
    pwm_set_enabled(motor_pwm_slice, true);
}

void enable_motor_power()
{
    // Check if the motor power control has been initialised
    if (!is_motor_pwr_ctrl_initialised)
    {
        log(LogLevel::ERROR, "Motor power control not initialised. Call init_motor_pwr_ctrl() first.");
        return;
    }

    // Check for motor fault
    if (is_motor_fault_active())
    {
        log(LogLevel::ERROR, "Motor fault detected, over-current or high temperature detected.");
        return;
    }

    gpio_put(MOTOR_PWR_CTRL_PIN, true);
    is_power_enabled = true;
    log(LogLevel::INFORMATION, "Motor power enabled.");
}

void disable_motor_power()
{
    gpio_put(MOTOR_PWR_CTRL_PIN, false);
    is_power_enabled = false;
    log(LogLevel::INFORMATION, "Motor power disabled.");
}

bool is_motor_fault_active()
{
    // low on fault
    return gpio_get(MOTOR_FAULT_FLAG_PIN) == 0;
}

void set_motor_position(ServoPosition position)
{
    switch (position)
    {
    case LEFT:
        pwm_set_gpio_level(TEST_MOTOR_IN_PIN, MOTOR_LEFT_PULSE_US);
        log(LogLevel::INFORMATION, "Motor set to LEFT position.");
        break;
    case CENTRE:
        pwm_set_gpio_level(TEST_MOTOR_IN_PIN, MOTOR_CENTRE_PULSE_US);
        log(LogLevel::INFORMATION, "Motor set to CENTRE position.");
        break;
    case RIGHT:
        pwm_set_gpio_level(TEST_MOTOR_IN_PIN, MOTOR_RIGHT_PULSE_US);
        log(LogLevel::INFORMATION, "Motor set to RIGHT position.");
        break;
    default:
        log(LogLevel::ERROR, "Invalid motor position specified.");
        break;
    }
}
