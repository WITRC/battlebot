/**
 * @file motor_bi.h
 * @brief Bidirectional ESC motor driver (Brake Type 1.2 "Reverse Brake").
 *
 * Controls ESCs that use a separate reverse-brake signal wire instead of
 * reversing the throttle pulse. Implements the `motor_t` interface.
 */
#ifndef MOTOR_BI_H
#define MOTOR_BI_H

#include "motor.h"
#include "motor_pwm_control.h"

#define MAX_PWM_US 1940 /**< Maximum throttle pulse width (µs). */
#define MIN_PWM_US 1100 /**< Minimum throttle pulse width (µs). */

/** Bidirectional motor state. `base` must remain the first field. */
typedef struct {
    motor_t base;               /**< Polymorphic base — must be first. */

    motor_pwm_t throttle_pwm;   /**< Throttle signal output. */
    motor_pwm_t reverse_pwm;    /**< Reverse-brake signal output. */

    uint16_t thr_min_us;        /**< Throttle pulse width at full stop (µs). */
    uint16_t thr_max_us;        /**< Throttle pulse width at full speed (µs). */

    uint16_t rev_fwd_us;        /**< Reverse wire pulse for forward (µs). */
    uint16_t rev_rev_us;        /**< Reverse wire pulse for reverse (µs). */

    int8_t last_dir;            /**< Last direction: -1 reverse, 0 stopped, +1 forward. */
} motor_bi_t;

/**
 * Constructor for bidirectional motor.
 *
 * @param m                   motor object
 * @param throttle_gpio       GPIO for throttle signal
 * @param reverse_gpio        GPIO for reverse-brake signal
 */
void motor_bi_ctor(motor_bi_t *m,
                   uint throttle_gpio,
                   uint reverse_gpio);

#endif // MOTOR_BI_H
