//
// Created by griffty on 2/19/26.
//

// motor_bi.h
#ifndef MOTOR_BI_H
#define MOTOR_BI_H

#include "motor.h"
#include "motor_pwm_control.h"

#define MAX_PWM_US 1940
#define MIN_PWM_US 1100

// Bidirectional ESC using separate reverse wire (Brake Type 1.2 "Reverse Brake")
typedef struct {
    motor_t base;               // MUST be first (inheritance)

    motor_pwm_t throttle_pwm;   // throttle signal output
    motor_pwm_t reverse_pwm;    // reverse-brake signal output

    uint16_t thr_min_us;
    uint16_t thr_max_us;

    uint16_t rev_fwd_us;
    uint16_t rev_rev_us;


    int8_t last_dir;              // -1 reverse, 0 stopped, +1 forward
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
