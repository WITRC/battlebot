// motor_servo_pwm.h
#ifndef MOTOR_PWM_CONTROL_H
#define MOTOR_PWM_CONTROL_H

#include "pico/stdlib.h"

#define PWM_WRAP     20000  // 20ms period = 50Hz
#define PWM_DIVIDER  150.0f // Clock divider

typedef struct {
    uint gpio;
    uint slice;
    uint chan;
} motor_pwm_t;

void servo_pwm_init_50hz_1mhz(motor_pwm_t *s, uint gpio);
void servo_pwm_write_us(motor_pwm_t *s, uint16_t us);

#endif
