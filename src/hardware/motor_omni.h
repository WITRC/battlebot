/**
 * @file motor_omni.h
 * @brief Unidirectional (omni/weapon) ESC motor driver.
 *
 * For ESCs that accept a standard 1000–2000 µs servo pulse with no reverse
 * signal (e.g., brushless weapon spinners). Implements the `motor_t` interface.
 */
#ifndef MOTOR_OMNI_H
#define MOTOR_OMNI_H

#include "motor.h"
#include "motor_pwm_control.h"

#define MAX_PWM_US 2000 /**< Full-speed pulse width (µs). */
#define MIN_PWM_US 1000 /**< Zero-speed / arming pulse width (µs). */

/** Unidirectional motor state. `base` must remain the first field. */
typedef struct {
    motor_t base;       /**< Polymorphic base — must be first. */
    motor_pwm_t pwm;    /**< PWM signal output. */

    uint16_t min_us;    /**< Pulse width at zero speed (µs). */
    uint16_t max_us;    /**< Pulse width at full speed (µs). */
} motor_omni_t;

/**
 * @brief Construct an omni motor on the given GPIO.
 * @param gpio  GPIO pin connected to the ESC signal wire.
 */
void motor_omni_ctor(motor_omni_t *m, uint gpio);

#endif
