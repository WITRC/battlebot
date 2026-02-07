#include "unity.h"
#include "motor_controller.h"
#include "test_helpers.h"
#include "mock_pico_hardware.h"

static motor_controller_t mc;

void setUp(void) {
    mock_reset();

    // Initialize motor controller struct without calling motor_controller_init
    // (which has hardware dependencies)
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
// Test tank drive
// ============================================================================

void test_tank_drive_forward(void) {
    motor_controller_tank_drive(&mc, 50, 50);

    TEST_ASSERT_EQUAL_INT(50, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(50, mc.right_speed);

    // Check PWM levels (50% forward = 1750us for bidirectional)
    TEST_ASSERT_EQUAL_UINT16(1750, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
    TEST_ASSERT_EQUAL_UINT16(1750, mock_get_pwm_level(PIN_MOTOR_RIGHT_FRONT));
}

void test_tank_drive_reverse(void) {
    motor_controller_tank_drive(&mc, -50, -50);

    TEST_ASSERT_EQUAL_INT(-50, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(-50, mc.right_speed);

    // Check PWM levels (50% reverse = 1250us for bidirectional)
    TEST_ASSERT_EQUAL_UINT16(1250, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
    TEST_ASSERT_EQUAL_UINT16(1250, mock_get_pwm_level(PIN_MOTOR_RIGHT_FRONT));
}

void test_tank_drive_turn_in_place(void) {
    motor_controller_tank_drive(&mc, 50, -50);

    TEST_ASSERT_EQUAL_INT(50, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(-50, mc.right_speed);

    // Left forward, right reverse
    TEST_ASSERT_EQUAL_UINT16(1750, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
    TEST_ASSERT_EQUAL_UINT16(1250, mock_get_pwm_level(PIN_MOTOR_RIGHT_FRONT));
}

// ============================================================================
// Test arcade drive mixing
// ============================================================================

void test_arcade_drive_forward_only(void) {
    motor_controller_arcade_drive(&mc, 100, 0);

    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(100, mc.right_speed);
}

void test_arcade_drive_turn_right_forward(void) {
    motor_controller_arcade_drive(&mc, 100, 50);

    // left = 100 + 50 = 150 (normalized to 100)
    // right = 100 - 50 = 50 (normalized to 33)
    // Normalization preserves ratio: 150:50 = 3:1 = 100:33
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(33, mc.right_speed);
}

void test_arcade_drive_turn_left_forward(void) {
    motor_controller_arcade_drive(&mc, 100, -50);

    // left = 100 - 50 = 50 (normalized to 33)
    // right = 100 + 50 = 150 (normalized to 100)
    TEST_ASSERT_EQUAL_INT(33, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(100, mc.right_speed);
}

void test_arcade_drive_turn_in_place_right(void) {
    motor_controller_arcade_drive(&mc, 0, 100);

    // left = 0 + 100 = 100
    // right = 0 - 100 = -100
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(-100, mc.right_speed);
}

void test_arcade_drive_normalization_exceeds_max(void) {
    motor_controller_arcade_drive(&mc, 100, 100);

    // left = 100 + 100 = 200 (normalized to 100)
    // right = 100 - 100 = 0 (normalized to 0)
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(0, mc.right_speed);
}

void test_arcade_drive_normalization_preserves_ratio(void) {
    motor_controller_arcade_drive(&mc, 80, 80);

    // left = 80 + 80 = 160 (normalized to 100)
    // right = 80 - 80 = 0 (normalized to 0)
    // Ratio 160:0 preserved as 100:0
    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
    TEST_ASSERT_EQUAL_INT(0, mc.right_speed);
}

// ============================================================================
// Test deadband
// ============================================================================

void test_set_left_applies_deadband(void) {
    motor_controller_set_left(&mc, 5);

    // 5 < MOTOR_DEADBAND (10), should be zeroed
    TEST_ASSERT_EQUAL_INT(0, mc.left_speed);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
}

void test_set_right_applies_deadband(void) {
    motor_controller_set_right(&mc, 15);

    // 15 > MOTOR_DEADBAND (10), should pass through
    TEST_ASSERT_EQUAL_INT(15, mc.right_speed);
}

// ============================================================================
// Test clamping
// ============================================================================

void test_set_left_clamps_to_max(void) {
    motor_controller_set_left(&mc, 150);

    TEST_ASSERT_EQUAL_INT(100, mc.left_speed);
}

void test_set_right_clamps_to_min(void) {
    motor_controller_set_right(&mc, -150);

    TEST_ASSERT_EQUAL_INT(-100, mc.right_speed);
}

// ============================================================================
// Test failsafe (SAFETY-CRITICAL)
// ============================================================================

void test_failsafe_not_triggered_with_recent_command(void) {
    motor_controller_set_left(&mc, 50);

    // Advance time by less than timeout
    mock_advance_time_us(400 * 1000);  // 400ms

    bool failsafe = motor_controller_check_failsafe(&mc);

    TEST_ASSERT_FALSE(failsafe);
    TEST_ASSERT_EQUAL_INT(50, mc.left_speed);  // Speed unchanged
}

void test_failsafe_triggers_after_timeout(void) {
    motor_controller_set_left(&mc, 50);

    // Advance time past timeout
    mock_advance_time_us(600 * 1000);  // 600ms

    bool failsafe = motor_controller_check_failsafe(&mc);

    TEST_ASSERT_TRUE(failsafe);
    TEST_ASSERT_EQUAL_INT(0, mc.left_speed);  // Motors stopped
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(PIN_MOTOR_LEFT_FRONT));
}

void test_failsafe_only_triggers_once(void) {
    motor_controller_set_left(&mc, 50);

    // Advance time and trigger failsafe
    mock_advance_time_us(600 * 1000);
    bool failsafe1 = motor_controller_check_failsafe(&mc);
    TEST_ASSERT_TRUE(failsafe1);

    // Check again without new command
    mock_advance_time_us(100 * 1000);
    bool failsafe2 = motor_controller_check_failsafe(&mc);
    TEST_ASSERT_TRUE(failsafe2);  // Still in failsafe
    TEST_ASSERT_TRUE(mc.failsafe_triggered);  // Flag remains set
}

void test_failsafe_resets_on_new_command(void) {
    motor_controller_set_left(&mc, 50);

    // Trigger failsafe
    mock_advance_time_us(600 * 1000);
    motor_controller_check_failsafe(&mc);
    TEST_ASSERT_TRUE(mc.failsafe_triggered);

    // New command should reset failsafe
    motor_controller_set_left(&mc, 30);
    TEST_ASSERT_FALSE(mc.failsafe_triggered);
    TEST_ASSERT_EQUAL_INT(30, mc.left_speed);
}

void test_failsafe_disarms_weapon(void) {
    mc.weapon_armed = true;
    motor_controller_set_weapon(&mc, 50);

    // Trigger failsafe
    mock_advance_time_us(600 * 1000);
    motor_controller_check_failsafe(&mc);

    TEST_ASSERT_FALSE(mc.weapon_armed);
    TEST_ASSERT_EQUAL_INT(0, mc.weapon_speed);
}

// ============================================================================
// Test weapon control
// ============================================================================

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

    // Weapon should clamp to 0 (no negative speeds)
    TEST_ASSERT_EQUAL_INT(0, mc.weapon_speed);
    TEST_ASSERT_EQUAL_UINT16(1500, mock_get_pwm_level(PIN_WEAPON));
}

void test_weapon_speed_set(void) {
    mc.weapon_armed = true;
    motor_controller_set_weapon(&mc, 75);

    TEST_ASSERT_EQUAL_INT(75, mc.weapon_speed);
    // 75% bidirectional = 1875us
    TEST_ASSERT_EQUAL_UINT16(1875, mock_get_pwm_level(PIN_WEAPON));
}
