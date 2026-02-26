/**
 * @file motor_pwm_control.c
 * @brief 50 Hz servo-style PWM implementation for ESC signal lines.
 */
#include "motor_pwm_control.h"

#ifndef UNIT_TESTING
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#endif

void servo_pwm_init_50hz_1mhz(motor_pwm_t *s, uint gpio) {
    s->gpio = gpio;

    gpio_set_function(gpio, GPIO_FUNC_PWM);
    s->slice = pwm_gpio_to_slice_num(gpio);
    s->chan  = pwm_gpio_to_channel(gpio);

    // // Make PWM counter tick at 1 MHz => 1 tick = 1 us
    // float div = (float)clock_get_hz(clk_sys) / 1000000.0f;

    pwm_set_wrap(s->slice, PWM_WRAP - 1);
    pwm_set_clkdiv(s->slice, PWM_DIVIDER);

    // 20ms period at 1MHz => wrap=20000 counts

    pwm_set_enabled(s->slice, true);
}

void servo_pwm_write_us(motor_pwm_t *s, uint16_t us) {
    pwm_set_chan_level(s->slice, s->chan, us);
}
