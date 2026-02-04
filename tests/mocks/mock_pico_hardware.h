#ifndef MOCK_PICO_HARDWARE_H
#define MOCK_PICO_HARDWARE_H

#include <stdint.h>
#include <stdbool.h>

// Mock Pico SDK types
typedef unsigned int uint;

// Mock hardware_pwm functions
void pwm_set_gpio_level(uint gpio, uint16_t level);
void pwm_set_chan_level(uint slice_num, uint channel, uint16_t level);
uint pwm_gpio_to_slice_num(uint gpio);
uint pwm_gpio_to_channel(uint gpio);
void pwm_set_wrap(uint slice_num, uint16_t wrap);
void pwm_set_enabled(uint slice_num, bool enabled);
void pwm_set_clkdiv(uint slice_num, float divider);

// Mock gpio functions
void gpio_set_function(uint gpio, uint fn);

// Mock time functions
uint64_t time_us_64(void);
uint32_t to_ms_since_boot(uint64_t t);
void sleep_ms(uint32_t ms);

// Mock absolute_time_t for compatibility
typedef uint64_t absolute_time_t;
absolute_time_t get_absolute_time(void);

// Constants
#define GPIO_FUNC_PWM 4

// Test helpers
void mock_reset(void);
void mock_advance_time_us(uint64_t us);
uint16_t mock_get_pwm_level(uint gpio);

#endif // MOCK_PICO_HARDWARE_H
