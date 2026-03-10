// =============================================================================
// Cooling Fan Driver
// Simple PWM control for a DC fan on Raspberry Pi Pico 2 W
// =============================================================================

#include <stdint.h>

#include "fan.h"
#include "../config/config.h"

#ifdef UNIT_TESTING
    #include "mock_pico_hardware.h"
#else
    #include "hardware/pwm.h"
    #include "hardware/gpio.h"
#endif

// Use a reasonably high PWM frequency to avoid audible noise.
// With default 125MHz system clock:
//   freq = 125MHz / (wrap * divider)
// Here: 125e6 / (1000 * 5) = 25kHz
#define FAN_PWM_WRAP     1000u
#define FAN_PWM_DIVIDER  5.0f

static uint32_t fan_slice_num;
static uint32_t fan_channel;

void fan_init(void) {
    // Configure GPIO for PWM
    gpio_set_function(PIN_FAN, GPIO_FUNC_PWM);

    // Determine PWM slice and channel
    uint32_t fan_slice_num = pwm_gpio_to_slice_num(PIN_FAN);
    uint32_t fan_channel =  pwm_gpio_to_channel(PIN_FAN);

    // Configure PWM for ~25kHz operation
    pwm_set_wrap(fan_slice_num, FAN_PWM_WRAP);
    pwm_set_clkdiv(fan_slice_num, FAN_PWM_DIVIDER);

    // Start with fan OFF
    pwm_set_chan_level(fan_slice_num, fan_channel, 0);
    pwm_set_enabled(fan_slice_num, true);
}

void fan_set_speed_percent(int percent) {
    if (percent > 100) percent = 100;
    if (percent < 0) percent = 0;

    uint32_t level = (percent * FAN_PWM_WRAP) / 100;
    pwm_set_chan_level(fan_slice_num, fan_channel, level);
}

void fan_on(void) {
    fan_set_speed_percent(100);
}

void fan_off(void) {
    fan_set_speed_percent(0);
}