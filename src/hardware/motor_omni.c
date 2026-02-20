//
// Created by griffty on 2/19/26.
//

#include <stdio.h>
#include "motor_omni.h"

#include "utility.h"

#define OMNI_MIN_PWM_US 1000
#define OMNI_MAX_PWM_US 2000

// ---- vtable methods ----
static void omni_init(motor_t *bm) {
    motor_omni_t *m = (motor_omni_t *)(bm);

    servo_pwm_init_50hz_1mhz(&m->pwm, m->pwm.gpio);
    servo_pwm_write_us(&m->pwm, m->min_us);

    printf("Omni motor init GPIO %u (slice %u chan %u) min=%u max=%u\n",
           m->pwm.gpio, m->pwm.slice, m->pwm.chan, m->min_us, m->max_us);
}

static void omni_set_speed(motor_t *bm, int speed) {
    motor_omni_t *m = (motor_omni_t *)(bm);

    uint16_t us = map_range_int(speed, 0, MOTOR_MAX_SPEED, m->min_us, m->max_us);
    servo_pwm_write_us(&m->pwm, us);
}

static void omni_stop(motor_t *bm) {
    motor_omni_t *m = (motor_omni_t *)(bm);
    servo_pwm_write_us(&m->pwm, m->min_us);
}

static const motor_vtbl_t OMNI_VTBL = {
    .init = omni_init,
    .set_speed = omni_set_speed,
    .stop = omni_stop,
};

void motor_omni_ctor(motor_omni_t *m, uint gpio) {
    m->base.v = &OMNI_VTBL;
    m->pwm.gpio = gpio;
    m->min_us = OMNI_MIN_PWM_US;
    m->max_us = OMNI_MAX_PWM_US;
}
