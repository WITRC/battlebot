#include "motor.h"
#include "config.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/utility.h"

void motor_init(motor_t* motor) {
    // Configure GPIO for PWM
    gpio_set_function(motor->gpio_pin, GPIO_FUNC_PWM);

    // Get PWM slice and channel for this pin
    motor->slice_num = pwm_gpio_to_slice_num(motor->gpio_pin);
    motor->channel = pwm_gpio_to_channel(motor->gpio_pin);

    // Configure PWM for 50Hz with 1us resolution
    pwm_set_wrap(motor->slice_num, PWM_WRAP - 1);
    pwm_set_clkdiv(motor->slice_num, PWM_DIVIDER);

    // Enable PWM first
    pwm_set_enabled(motor->slice_num, true);

    motor_set_speed(motor, 0);

    printf("Motor initialized on GPIO %d (slice %d, channel %d)\n",
           motor->gpio_pin, motor->slice_num, motor->channel);
}


void motor_set_pulse_us(motor_t* motor, uint16_t us) {
    uint16_t level = clamp_int(us, ESC_ABS_MIN_US, ESC_ABS_MAX_US);

    static uint16_t last_level[8] = {0};
    if (level != last_level[motor->gpio_pin] && motor->gpio_pin < 8) {
        pwm_set_chan_level(motor->slice_num, motor->channel, level);
        last_level[motor->gpio_pin] = level;
        printf("GPIO%d: %dus\n", motor->gpio_pin, us);
    }
}


void motor_set_speed(motor_t* m, int speed) {
    //todo: test with motors, rest might not be middle
    int min = m->bidirectional ? -100 : 0;
    speed = clamp_int(speed, min, 100);
    int us = map_range_int(speed,min,100,m->min_us,m->max_us);
    motor_set_pulse_us(m, us);
}

void motor_stop(motor_t* motor) {
    //todo: test with motors, rest might not be middle
    if (motor->bidirectional) {
        motor_set_pulse_us(motor, motor->min_us + (motor->max_us - motor->min_us) / 2);
    } else {
        motor_set_pulse_us(motor, motor->min_us);
    }
}

void get_wheel_motor(motor_t* m, uint pin)  {
    *m = (motor_t){pin, 0, 0, WHEEL_MOTOR_MIN_US, WHEEL_MOTOR_MAX_US, true};
    motor_init(m);
}

void get_weapon_motor(motor_t* m, uint pin)  {
    *m = (motor_t){pin, 0, 0, WEAPON_MOTOR_MIN_US, WEAPON_MOTOR_MAX_US, false};
    motor_init(m);
}