/**
 * @file motor_pwm_control.h
 * @brief 50 Hz servo-style PWM output for ESC control.
 */
#ifndef MOTOR_PWM_CONTROL_H
#define MOTOR_PWM_CONTROL_H

#include "pico/stdlib.h"

#define PWM_WRAP     20000  /**< Wrap value giving a 20 ms (50 Hz) period. */
#define PWM_DIVIDER  150.0f /**< Clock divider to reach ~1 MHz PWM clock. */

/** PWM channel state for a single ESC signal line. */
typedef struct {
    uint gpio;  /**< GPIO pin number. */
    uint slice; /**< PWM slice index. */
    uint chan;  /**< PWM channel within the slice. */
} motor_pwm_t;

/** @brief Configure a GPIO for 50 Hz servo PWM at ~1 MHz resolution. */
void servo_pwm_init_50hz_1mhz(motor_pwm_t *s, uint gpio);

/** @brief Write a pulse width in microseconds to the ESC signal line. */
void servo_pwm_write_us(motor_pwm_t *s, uint16_t us);

#endif
