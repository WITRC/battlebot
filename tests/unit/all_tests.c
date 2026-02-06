#include "unity.h"
#include "motor.h"
#include "motor_controller.h"
#include "config.h"
#include "test_helpers.h"
#include "mock_pico_hardware.h"

static motor_controller_t mc;

void setUp(void) {
    mock_reset();

    // Initialize motor controller struct for controller tests
    mc.left_speed = 0;
    mc.right_speed = 0;
    mc.weapon_speed = 0;
    mc.weapon_armed = false;
    mc.last_command_time_ms = 0;
    mc.failsafe_triggered = false;

    // Initialize motor structs with test values
    mc.motor_left_front.gpio_pin = PIN_MOTOR_LEFT_FRONT;
    mc.motor_left_front.slice_num = pwm_gpio_to_slice_num(PIN_MOTOR_LEFT_FRONT);
    mc.motor_left_front.channel = pwm_gpio_to_channel(PIN_MOTOR_LEFT_FRONT);
    mc.motor_left_front.min_us = DRIVE_MIN_US;
    mc.motor_left_front.mid_us = DRIVE_MID_US;
    mc.motor_left_front.max_us = DRIVE_MAX_US;

    mc.motor_left_back.gpio_pin = PIN_MOTOR_LEFT_BACK;
    mc.motor_left_back.slice_num = pwm_gpio_to_slice_num(PIN_MOTOR_LEFT_BACK);
    mc.motor_left_back.channel = pwm_gpio_to_channel(PIN_MOTOR_LEFT_BACK);
    mc.motor_left_back.min_us = DRIVE_MIN_US;
    mc.motor_left_back.mid_us = DRIVE_MID_US;
    mc.motor_left_back.max_us = DRIVE_MAX_US;

    mc.motor_right_front.gpio_pin = PIN_MOTOR_RIGHT_FRONT;
    mc.motor_right_front.slice_num = pwm_gpio_to_slice_num(PIN_MOTOR_RIGHT_FRONT);
    mc.motor_right_front.channel = pwm_gpio_to_channel(PIN_MOTOR_RIGHT_FRONT);
    mc.motor_right_front.min_us = DRIVE_MIN_US;
    mc.motor_right_front.mid_us = DRIVE_MID_US;
    mc.motor_right_front.max_us = DRIVE_MAX_US;

    mc.motor_right_back.gpio_pin = PIN_MOTOR_RIGHT_BACK;
    mc.motor_right_back.slice_num = pwm_gpio_to_slice_num(PIN_MOTOR_RIGHT_BACK);
    mc.motor_right_back.channel = pwm_gpio_to_channel(PIN_MOTOR_RIGHT_BACK);
    mc.motor_right_back.min_us = DRIVE_MIN_US;
    mc.motor_right_back.mid_us = DRIVE_MID_US;
    mc.motor_right_back.max_us = DRIVE_MAX_US;

    mc.weapon.gpio_pin = PIN_WEAPON;
    mc.weapon.slice_num = pwm_gpio_to_slice_num(PIN_WEAPON);
    mc.weapon.channel = pwm_gpio_to_channel(PIN_WEAPON);
    mc.weapon.min_us = WEAPON_MIN_US;
    mc.weapon.mid_us = WEAPON_MID_US;
    mc.weapon.max_us = WEAPON_MAX_US;
}

void tearDown(void) {}

// ============================================================================
// MOTOR TESTS
// ============================================================================

void test_motor_set_throttle_zero(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_throttle(&motor, 0.0f);
    TEST_ASSERT_EQUAL_UINT16(1000, mock_get_pwm_level(0));
}

void test_motor_set_throttle_half(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_throttle(&motor, 0.5f);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(0));
}

void test_motor_set_throttle_full(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_throttle(&motor, 1.0f);
    TEST_ASSERT_EQUAL_UINT16(2000, mock_get_pwm_level(0));
}

void test_motor_set_throttle_clamps_low(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_throttle(&motor, -0.5f);
    TEST_ASSERT_EQUAL_UINT16(1000, mock_get_pwm_level(0));
}

void test_motor_set_throttle_clamps_high(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_throttle(&motor, 2.0f);
    TEST_ASSERT_EQUAL_UINT16(2000, mock_get_pwm_level(0));
}

void test_motor_set_speed_bidirectional_zero(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_speed(&motor, 0, true);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(0));
}

void test_motor_set_speed_bidirectional_forward_full(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_speed(&motor, 100, true);
    TEST_ASSERT_EQUAL_UINT16(2000, mock_get_pwm_level(0));
}

void test_motor_set_speed_bidirectional_reverse_full(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_speed(&motor, -100, true);
    TEST_ASSERT_EQUAL_UINT16(1000, mock_get_pwm_level(0));
}

void test_motor_set_speed_bidirectional_forward_half(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_speed(&motor, 50, true);
    TEST_ASSERT_EQUAL_UINT16(1750, mock_get_pwm_level(0));
}

void test_motor_set_speed_unidirectional_zero(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_speed(&motor, 0, false);
    TEST_ASSERT_EQUAL_UINT16(1000, mock_get_pwm_level(0));
}

void test_motor_set_speed_unidirectional_full(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_speed(&motor, 100, false);
    TEST_ASSERT_EQUAL_UINT16(2000, mock_get_pwm_level(0));
}

void test_motor_set_speed_unidirectional_negative_abs(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_speed(&motor, -50, false);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(0));
}

void test_motor_set_pulse_clamps_to_abs_min(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_pulse_us(&motor, 800);
    TEST_ASSERT_EQUAL_UINT16(ESC_ABS_MIN_US, mock_get_pwm_level(0));
}

void test_motor_set_pulse_clamps_to_abs_max(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_pulse_us(&motor, 2500);
    TEST_ASSERT_EQUAL_UINT16(ESC_ABS_MAX_US, mock_get_pwm_level(0));
}

void test_motor_set_pulse_normal_range(void) {
    motor_t motor = {
        .gpio_pin = 0, .slice_num = 0, .channel = 0,
        .min_us = 1000, .mid_us = 1500, .max_us = 2000
    };
    motor_set_pulse_us(&motor, 1500);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(0));
}

// ============================================================================
// MOTOR CONTROLLER TESTS
// ============================================================================

void test_tank_drive_forward(void) {
    motor_controller_tank_drive(&mc, 50, 50);
    TEST_ASSERT_EQUAL_INT(50, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(50, mc.right_speed);
    TEST_ASSERT_EQUAL_UINT16(1750, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
    TEST_ASSERT_EQUAL_UINT16(1750, mock_get_pwm_level(PIN_MOTOR_RIGHT_FRONT));
}

void test_tank_drive_reverse(void) {
    motor_controller_tank_drive(&mc, -50, -50);
    TEST_ASSERT_EQUAL_INT(-50, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(-50, mc.right_speed);
    TEST_ASSERT_EQUAL_UINT16(1250, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
    TEST_ASSERT_EQUAL_UINT16(1250, mock_get_pwm_level(PIN_MOTOR_RIGHT_FRONT));
}

void test_tank_drive_turn_in_place(void) {
    motor_controller_tank_drive(&mc, 50, -50);
    TEST_ASSERT_EQUAL_INT(50, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(-50, mc.right_speed);
    TEST_ASSERT_EQUAL_UINT16(1750, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
    TEST_ASSERT_EQUAL_UINT16(1250, mock_get_pwm_level(PIN_MOTOR_RIGHT_FRONT));
}

void test_arcade_drive_forward_only(void) {
    motor_controller_arcade_drive(&mc, 100, 0);
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(100, mc.right_speed);
}

void test_arcade_drive_turn_right_forward(void) {
    motor_controller_arcade_drive(&mc, 100, 50);
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(33, mc.right_speed);
}

void test_arcade_drive_turn_left_forward(void) {
    motor_controller_arcade_drive(&mc, 100, -50);
    TEST_ASSERT_EQUAL_INT(33, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(100, mc.right_speed);
}

void test_arcade_drive_turn_in_place_right(void) {
    motor_controller_arcade_drive(&mc, 0, 100);
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(-100, mc.right_speed);
}

void test_arcade_drive_normalization_exceeds_max(void) {
    motor_controller_arcade_drive(&mc, 100, 100);
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(0, mc.right_speed);
}

void test_arcade_drive_normalization_preserves_ratio(void) {
    motor_controller_arcade_drive(&mc, 80, 80);
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(0, mc.right_speed);
}

void test_set_left_applies_deadband(void) {
    motor_controller_set_left(&mc, 5);
    TEST_ASSERT_EQUAL_INT(0, mc.left_speed);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
}

void test_set_right_applies_deadband(void) {
    motor_controller_set_right(&mc, 15);
    TEST_ASSERT_EQUAL_INT(15, mc.right_speed);
}

void test_set_left_clamps_to_max(void) {
    motor_controller_set_left(&mc, 150);
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
}

void test_set_right_clamps_to_min(void) {
    motor_controller_set_right(&mc, -150);
    TEST_ASSERT_EQUAL_INT(-100, mc.right_speed);
}

void test_failsafe_not_triggered_with_recent_command(void) {
    motor_controller_set_left(&mc, 50);
    mock_advance_time_us(400 * 1000);
    bool failsafe = motor_controller_check_failsafe(&mc);
    TEST_ASSERT_FALSE(failsafe);
    TEST_ASSERT_EQUAL_INT(50, mc.left_speed);
}

void test_failsafe_triggers_after_timeout(void) {
    motor_controller_set_left(&mc, 50);
    mock_advance_time_us(600 * 1000);
    bool failsafe = motor_controller_check_failsafe(&mc);
    TEST_ASSERT_TRUE(failsafe);
    TEST_ASSERT_EQUAL_INT(0, mc.left_speed);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
}

void test_failsafe_only_triggers_once(void) {
    motor_controller_set_left(&mc, 50);
    mock_advance_time_us(600 * 1000);
    motor_controller_check_failsafe(&mc);
    mock_advance_time_us(100 * 1000);
    bool failsafe = motor_controller_check_failsafe(&mc);
    TEST_ASSERT_TRUE(failsafe);
    TEST_ASSERT_TRUE(mc.failsafe_triggered);
}

void test_failsafe_resets_on_new_command(void) {
    motor_controller_set_left(&mc, 50);
    mock_advance_time_us(600 * 1000);
    motor_controller_check_failsafe(&mc);
    TEST_ASSERT_TRUE(mc.failsafe_triggered);
    motor_controller_set_left(&mc, 30);
    TEST_ASSERT_FALSE(mc.failsafe_triggered);
    TEST_ASSERT_EQUAL_INT(30, mc.left_speed);
}

void test_failsafe_disarms_weapon(void) {
    mc.weapon_armed = true;
    motor_controller_set_weapon(&mc, 50);
    mock_advance_time_us(600 * 1000);
    motor_controller_check_failsafe(&mc);
    TEST_ASSERT_FALSE(mc.weapon_armed);
    TEST_ASSERT_EQUAL_INT(0, mc.weapon_speed);
}

void test_weapon_arm_disarm_cycle(void) {
    TEST_ASSERT_FALSE(mc.weapon_armed);
    motor_controller_arm_weapon(&mc);
    TEST_ASSERT_TRUE(mc.weapon_armed);
    motor_controller_disarm_weapon(&mc);
    TEST_ASSERT_FALSE(mc.weapon_armed);
}

void test_weapon_clamps_to_zero_to_100(void) {
    mc.weapon_armed = true;
    motor_controller_set_weapon(&mc, -50);
    TEST_ASSERT_EQUAL_INT(0, mc.weapon_speed);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(PIN_WEAPON));
}

void test_weapon_speed_set(void) {
    mc.weapon_armed = true;
    motor_controller_set_weapon(&mc, 75);
    TEST_ASSERT_EQUAL_INT(75, mc.weapon_speed);
    TEST_ASSERT_EQUAL_UINT16(1875, mock_get_pwm_level(PIN_WEAPON));
}

// ============================================================================
// TEST RUNNER
// ============================================================================

int main(void) {
    UNITY_BEGIN();

    // Motor tests
    RUN_TEST(test_motor_set_throttle_zero);
    RUN_TEST(test_motor_set_throttle_half);
    RUN_TEST(test_motor_set_throttle_full);
    RUN_TEST(test_motor_set_throttle_clamps_low);
    RUN_TEST(test_motor_set_throttle_clamps_high);
    RUN_TEST(test_motor_set_speed_bidirectional_zero);
    RUN_TEST(test_motor_set_speed_bidirectional_forward_full);
    RUN_TEST(test_motor_set_speed_bidirectional_reverse_full);
    RUN_TEST(test_motor_set_speed_bidirectional_forward_half);
    RUN_TEST(test_motor_set_speed_unidirectional_zero);
    RUN_TEST(test_motor_set_speed_unidirectional_full);
    RUN_TEST(test_motor_set_speed_unidirectional_negative_abs);
    RUN_TEST(test_motor_set_pulse_clamps_to_abs_min);
    RUN_TEST(test_motor_set_pulse_clamps_to_abs_max);
    RUN_TEST(test_motor_set_pulse_normal_range);

    // Motor controller tests
    RUN_TEST(test_tank_drive_forward);
    RUN_TEST(test_tank_drive_reverse);
    RUN_TEST(test_tank_drive_turn_in_place);
    RUN_TEST(test_arcade_drive_forward_only);
    RUN_TEST(test_arcade_drive_turn_right_forward);
    RUN_TEST(test_arcade_drive_turn_left_forward);
    RUN_TEST(test_arcade_drive_turn_in_place_right);
    RUN_TEST(test_arcade_drive_normalization_exceeds_max);
    RUN_TEST(test_arcade_drive_normalization_preserves_ratio);
    RUN_TEST(test_set_left_applies_deadband);
    RUN_TEST(test_set_right_applies_deadband);
    RUN_TEST(test_set_left_clamps_to_max);
    RUN_TEST(test_set_right_clamps_to_min);
    RUN_TEST(test_failsafe_not_triggered_with_recent_command);
    RUN_TEST(test_failsafe_triggers_after_timeout);
    RUN_TEST(test_failsafe_only_triggers_once);
    RUN_TEST(test_failsafe_resets_on_new_command);
    RUN_TEST(test_failsafe_disarms_weapon);
    RUN_TEST(test_weapon_arm_disarm_cycle);
    RUN_TEST(test_weapon_clamps_to_zero_to_100);
    RUN_TEST(test_weapon_speed_set);

    return UNITY_END();
}
