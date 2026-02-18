#include "mock_pico_hardware.h"
#include <stdio.h>

// Simple mock implementations for testing
// In real tests, you can use CMock to generate these automatically

static uint16_t mock_pwm_levels[32] = {0};
static uint64_t mock_time_us = 0;

void pwm_set_gpio_level(uint gpio, uint16_t level) {
    if (gpio < 32) {
        mock_pwm_levels[gpio] = level;
    }
}

void pwm_set_chan_level(uint slice_num, uint channel, uint16_t level) {
    // Convert slice/channel back to GPIO pin
    uint gpio = slice_num * 2 + channel;
    pwm_set_gpio_level(gpio, level);
}

uint pwm_gpio_to_slice_num(uint gpio) {
    return gpio / 2;
}

uint pwm_gpio_to_channel(uint gpio) {
    return gpio % 2;
}

void pwm_set_wrap(uint slice_num, uint16_t wrap) {
    // Mock implementation
}

void pwm_set_enabled(uint slice_num, bool enabled) {
    // Mock implementation
}

void pwm_set_clkdiv(uint slice_num, float divider) {
    // Mock implementation
}

void gpio_set_function(uint gpio, uint fn) {
    // Mock implementation
}

uint64_t time_us_64(void) {
    return mock_time_us;
}

absolute_time_t get_absolute_time(void) {
    return mock_time_us;
}

uint32_t to_ms_since_boot(uint64_t t) {
    return (uint32_t)(t / 1000);
}

void sleep_ms(uint32_t ms) {
    mock_time_us += ms * 1000;
}

// Test helper to advance mock time
void mock_advance_time_us(uint64_t us) {
    mock_time_us += us;
}

// Test helper to reset mocks
void mock_reset(void) {
    for (int i = 0; i < 32; i++) {
        mock_pwm_levels[i] = 0;
    }
    mock_time_us = 0;
}

// Test helper to get PWM level
uint16_t mock_get_pwm_level(uint gpio) {
    return gpio < 32 ? mock_pwm_levels[gpio] : 0;
}
