#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <stdint.h>

// Mock time control
void mock_advance_time_us(uint64_t us);
void mock_reset(void);
uint16_t mock_get_pwm_level(unsigned int gpio);

#endif // TEST_HELPERS_H
