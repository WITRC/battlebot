#include "unity.h"
#include "motor.h"
#include "config.h"
#include "test_helpers.h"

void setUp(void) {
    mock_reset();
}

void tearDown(void) {}

// ============================================================================
// Test throttle mapping (motor_set_throttle)
// ============================================================================

void test_motor_set_throttle_zero(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_throttle(&motor, 0.0f);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1000, pwm);
}

void test_motor_set_throttle_half(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_throttle(&motor, 0.5f);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1500, pwm);
}

void test_motor_set_throttle_full(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_throttle(&motor, 1.0f);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(2000, pwm);
}

void test_motor_set_throttle_clamps_low(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_throttle(&motor, -0.5f);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1000, pwm);  // Clamped to 0.0
}

void test_motor_set_throttle_clamps_high(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_throttle(&motor, 2.0f);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(2000, pwm);  // Clamped to 1.0
}

// ============================================================================
// Test speed mapping bidirectional (motor_set_speed)
// ============================================================================

void test_motor_set_speed_bidirectional_zero(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_speed(&motor, 0, true);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1500, pwm);  // Mid = stopped
}

void test_motor_set_speed_bidirectional_forward_full(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_speed(&motor, 100, true);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(2000, pwm);  // Full forward
}

void test_motor_set_speed_bidirectional_reverse_full(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_speed(&motor, -100, true);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1000, pwm);  // Full reverse
}

void test_motor_set_speed_bidirectional_forward_half(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_speed(&motor, 50, true);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1750, pwm);  // Halfway between mid and max
}

// ============================================================================
// Test speed mapping unidirectional
// ============================================================================

void test_motor_set_speed_unidirectional_zero(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_speed(&motor, 0, false);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1000, pwm);  // Min = stopped
}

void test_motor_set_speed_unidirectional_full(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_speed(&motor, 100, false);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(2000, pwm);  // Full speed
}

void test_motor_set_speed_unidirectional_negative_abs(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_speed(&motor, -50, false);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1500, pwm);  // Absolute value: 50% = 1500us
}

// ============================================================================
// Test pulse width safety (motor_set_pulse_us)
// ============================================================================

void test_motor_set_pulse_clamps_to_abs_min(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_pulse_us(&motor, 800);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(ESC_ABS_MIN_US, pwm);  // Clamped to 900
}

void test_motor_set_pulse_clamps_to_abs_max(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_pulse_us(&motor, 2500);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(ESC_ABS_MAX_US, pwm);  // Clamped to 2100
}

void test_motor_set_pulse_normal_range(void) {
    motor_t motor = {
        .gpio_pin = 0,
        .slice_num = 0,
        .channel = 0,
        .min_us = 1000,
        .mid_us = 1500,
        .max_us = 2000
    };

    motor_set_pulse_us(&motor, 1500);

    uint16_t pwm = mock_get_pwm_level(0);
    TEST_ASSERT_EQUAL_UINT16(1500, pwm);  // No clamping
}
