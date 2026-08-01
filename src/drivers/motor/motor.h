#pragma once

#define CLK_SYS_HZ 125000000 // Default system clock frequency

// prac board
#define TEST_MOTOR_IN_PIN 12 

/**
 * @brief Predefined mechanical positions of the servo.
 */
enum ServoPosition
{
    LEFT,
    CENTRE,
    RIGHT
};

/**
 * @brief Initializes the motor's power control and the fault flag
 *
 * Power is disabled on initalisation
 */
void init_motor_pwr_ctrl();

/**
 * @brief Initializes the motor's PWM functionality
 *
 * The motor is set to the centre position on initalisation
 */
void init_motor();

/**
 * @brief Enable motor's power
 *
 * Will not enable if the motor power control has not been initialised
 *
 * Will not enable if a motor fault is detected (over-current or high temperature)
 */
void enable_motor_power();

/**
 * @brief Disable motor's power
 */
void disable_motor_power();

/**
 * @brief Check if a motor fault is active (over-current or high temperature)
 */
bool is_motor_fault_active();

/**
 * @brief Set's the motor's position
 * 
 * Note: motor will not move until the power is enabled
 * @param position The desired position of the motor (LEFT, CENTRE or RIGHT)
 */
void set_motor_position(ServoPosition position);