//
// Created by griffty on 2/19/26.
//
// motor_omni.h
#ifndef MOTOR_OMNI_H
#define MOTOR_OMNI_H

#include "motor.h"
#include "motor_pwm_control.h"

typedef struct {
    motor_t base;
    motor_pwm_t pwm;

    uint16_t min_us;
    uint16_t max_us;
} motor_omni_t;

void motor_omni_ctor(motor_omni_t *m, uint gpio);

#endif
