#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
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

#define MOTOR_LEFT_PULSE_US 1550
#define MOTOR_CENTRE_PULSE_US 1750
#define MOTOR_RIGHT_PULSE_US 1950

#define MOTOR_MOVE_TIME_MS 200
#define MOTOR_FAULT_CHECK_INTERVAL_MS 10

bool is_power_enabled = false;
bool is_motor_pwr_ctrl_initialised = false;

void init_motor_pwr_ctrl()
{
    gpio_init(MOTOR_PWR_CTRL_PIN);
    gpio_set_dir(MOTOR_PWR_CTRL_PIN, GPIO_OUT);
    gpio_put(MOTOR_PWR_CTRL_PIN, false); // Disable motor power on startup

    gpio_init(MOTOR_FAULT_FLAG_PIN);
    gpio_set_dir(MOTOR_FAULT_FLAG_PIN, GPIO_IN);
    is_motor_pwr_ctrl_initialised = true;
}

void init_motor()
{
    pwm_config config = pwm_get_default_config();
    int motor_pwm_slice = pwm_gpio_to_slice_num(MOTOR_IN_PIN);

    gpio_set_function(MOTOR_IN_PIN, GPIO_FUNC_PWM);

    // Slow down clock to COUNTER_HZ
    float clock_divider = CLK_SYS_HZ / COUNTER_HZ;
    pwm_config_set_clkdiv(&config, clock_divider);
    pwm_config_set_wrap(&config, PWM_PERIOD_US - 1);
    pwm_init(motor_pwm_slice, &config, false);

    pwm_set_gpio_level(MOTOR_IN_PIN, 0); // Set initial pulse width to 0 (motor off)
    pwm_set_enabled(motor_pwm_slice, true);
}

bool enable_motor()
{
    // Check if the motor power control has been initialised
    if (!is_motor_pwr_ctrl_initialised)
    {
        log(LogLevel::ERROR, "Motor power control not initialised.");
        pwm_set_gpio_level(MOTOR_IN_PIN, 0);
        return false;
    }

    // Check for motor fault
    if (is_motor_fault_active())
    {
        log(LogLevel::ERROR, "Motor fault, over-current or high temperature detected. Motor power disabled.");
        disable_motor();
        return false;
    }

    gpio_put(MOTOR_PWR_CTRL_PIN, true);
    is_power_enabled = true;
    return true;
}

void disable_motor()
{
    pwm_set_gpio_level(MOTOR_IN_PIN, 0);
    gpio_put(MOTOR_PWR_CTRL_PIN, false);
    is_power_enabled = false;
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
        pwm_set_gpio_level(MOTOR_IN_PIN, MOTOR_LEFT_PULSE_US);
        log(LogLevel::INFORMATION, "Motor set to LEFT position.");
        break;
    case CENTRE:
        pwm_set_gpio_level(MOTOR_IN_PIN, MOTOR_CENTRE_PULSE_US);
        log(LogLevel::INFORMATION, "Motor set to CENTRE position.");
        break;
    case RIGHT:
        pwm_set_gpio_level(MOTOR_IN_PIN, MOTOR_RIGHT_PULSE_US);
        log(LogLevel::INFORMATION, "Motor set to RIGHT position.");
        break;
    default:
        log(LogLevel::ERROR, "Invalid motor position specified.");
        break;
    }
}

void move_motor_position_safely(ServoPosition position)
{
    set_motor_position(position);
    if (!enable_motor())
    {
        return;
    }

    absolute_time_t end_time = make_timeout_time_ms(MOTOR_MOVE_TIME_MS);
    while (!time_reached(end_time))
    {
        if (is_motor_fault_active())
        {
            log(LogLevel::ERROR, "Motor fault, over-current or high temperature detected. Motor power disabled.");
            disable_motor();
            return;
        }
        sleep_ms(MOTOR_FAULT_CHECK_INTERVAL_MS);
    }
    disable_motor();
}
