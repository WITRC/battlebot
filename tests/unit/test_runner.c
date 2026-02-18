#include "unity.h"

// Motor tests
void test_motor_set_throttle_zero(void);
void test_motor_set_throttle_half(void);
void test_motor_set_throttle_full(void);
void test_motor_set_throttle_clamps_low(void);
void test_motor_set_throttle_clamps_high(void);
void test_motor_set_speed_bidirectional_zero(void);
void test_motor_set_speed_bidirectional_forward_full(void);
void test_motor_set_speed_bidirectional_reverse_full(void);
void test_motor_set_speed_bidirectional_forward_half(void);
void test_motor_set_speed_unidirectional_zero(void);
void test_motor_set_speed_unidirectional_full(void);
void test_motor_set_speed_unidirectional_negative_abs(void);
void test_motor_set_pulse_clamps_to_abs_min(void);
void test_motor_set_pulse_clamps_to_abs_max(void);
void test_motor_set_pulse_normal_range(void);

// Motor controller tests
void test_tank_drive_forward(void);
void test_tank_drive_reverse(void);
void test_tank_drive_turn_in_place(void);
void test_arcade_drive_forward_only(void);
void test_arcade_drive_turn_right_forward(void);
void test_arcade_drive_turn_left_forward(void);
void test_arcade_drive_turn_in_place_right(void);
void test_arcade_drive_normalization_exceeds_max(void);
void test_arcade_drive_normalization_preserves_ratio(void);
void test_set_left_applies_deadband(void);
void test_set_right_applies_deadband(void);
void test_set_left_clamps_to_max(void);
void test_set_right_clamps_to_min(void);
void test_failsafe_not_triggered_with_recent_command(void);
void test_failsafe_triggers_after_timeout(void);
void test_failsafe_only_triggers_once(void);
void test_failsafe_resets_on_new_command(void);
void test_failsafe_disarms_weapon(void);
void test_weapon_arm_disarm_cycle(void);
void test_weapon_clamps_to_zero_to_100(void);
void test_weapon_speed_set(void);

int main(void) {
    UNITY_BEGIN();

    // Motor tests (15 tests)
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

    // Motor controller tests (25 tests)
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
