// motor_bi.c
#include "motor_bi.h"

#define BI_MIN_PWM_US 1100
#define BI_MAX_PWM_US 1940

#include  <stdio.h>

#include "pico/stdlib.h" // sleep_ms
#include "utility.h"

static void bi_set_dir(motor_bi_t *m, int8_t dir) {
    if (dir == m->last_dir) return;

    if (dir >= 0) {
        servo_pwm_write_us(&m->reverse_pwm, m->rev_fwd_us);
    } else {
        servo_pwm_write_us(&m->reverse_pwm, m->rev_rev_us);
    }

    m->last_dir = dir;
}

// ---- vtable methods ----
static void bi_init(motor_t *bm) {
    motor_bi_t *m = (motor_bi_t *)(bm);

    servo_pwm_init_50hz_1mhz(&m->throttle_pwm, m->throttle_pwm.gpio);
    servo_pwm_init_50hz_1mhz(&m->reverse_pwm,  m->reverse_pwm.gpio);

    // Per ESC docs: reverse channel should be in 0-50% (0% preferred) at power-on
    servo_pwm_write_us(&m->reverse_pwm,  m->rev_fwd_us);
    servo_pwm_write_us(&m->throttle_pwm, m->thr_min_us);

    m->last_dir = 0;

    printf("Bi motor init THR GPIO %u (slice %u chan %u), REV GPIO %u (slice %u chan %u)\n",
           m->throttle_pwm.gpio, m->throttle_pwm.slice, m->throttle_pwm.chan,
           m->reverse_pwm.gpio,  m->reverse_pwm.slice,  m->reverse_pwm.chan);
}



static void bi_set_speed(motor_t *bm, int speed) {
    motor_bi_t *m = (motor_bi_t*)bm;

    speed = clamp_int(speed, -100, 100);

    int8_t dir = (speed > 0) ? +1 : (speed < 0) ? -1 : 0;
    int mag = (speed >= 0) ? speed : -speed; // 0..100

    if (mag == 0) {
        bi_set_dir(m, 0);
        servo_pwm_write_us(&m->throttle_pwm, m->thr_min_us);
        return;
    }

    bi_set_dir(m, dir);

    uint16_t thr_us = map_range_int(mag, 0, MOTOR_MAX_SPEED, m->thr_min_us, m->thr_max_us);
    servo_pwm_write_us(&m->throttle_pwm, thr_us);
}

static void bi_stop(motor_t *bm) {
    motor_bi_t *m = (motor_bi_t *)(bm);
    bi_set_dir(m, 0);
    servo_pwm_write_us(&m->throttle_pwm, m->thr_min_us);
}

static const motor_vtbl_t BI_VTBL = {
    .init = bi_init,
    .set_speed = bi_set_speed,
    .stop = bi_stop,
};

void motor_bi_ctor(motor_bi_t *m,
                   uint throttle_gpio,
                   uint reverse_gpio) {
    m->base.v = &BI_VTBL;

    m->throttle_pwm.gpio = throttle_gpio;
    m->reverse_pwm.gpio  = reverse_gpio;

    m->thr_min_us = BI_MIN_PWM_US;
    m->thr_max_us = BI_MAX_PWM_US;

    m->rev_fwd_us = BI_MIN_PWM_US;
    m->rev_rev_us = BI_MAX_PWM_US;

    m->last_dir = 0;
}