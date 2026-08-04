#pragma once

#define DEFAULT_MOTOR_MOVE_TIME_MS 200

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
void motor_pwr_ctrl_init();

/** 
 * @brief Initializes the motor's PWM functionality 
 */
void motor_init();

/**
 * @brief Enable motor's power
 *
 * @return true if the motor was successfully enabled, false if there was a fault or the power control was not initialised
 */
bool motor_enable();

/** 
 * @brief Disable motor's power and input signal 
 */
void motor_disable();

/** 
 * @brief Check if a motor fault is active (over-current or high temperature) 
 * */
bool is_motor_fault_active();

/**
 * @brief Set's the motor's position by setting input signal
 *
 * Note: motor will not move until the power is enabled
 * @param position The desired position of the motor (LEFT, CENTRE or RIGHT)
 */
void motor_set_position(ServoPosition position);

/**
 * @brief Move the motor's position safely
 * 
 * Sets input signal, enables motor, waits for motor to move while checking for faults then disables motor
 * 
 * Note: also is a blocking function which will need to be changed in future
 *
 * @param position The desired position of the motor (LEFT, CENTRE or RIGHT)
 * @param move_time_ms The time in milliseconds to wait for the motor to move before disabling power (default is minimum time to move from left to right)
 */
void motor_move_motor_safely(ServoPosition position, uint32_t move_time_ms = DEFAULT_MOTOR_MOVE_TIME_MS);