// =============================================================================
// Motor Controller Implementation
// Tank/arcade drive mixing, weapon control, and failsafe
// =============================================================================

#include "motor_controller.h"
#include <stdio.h>

#include "utility.h"
#include "pico/time.h"

#ifdef UNIT_TESTING
    #include "mock_pico_hardware.h"
#endif

/**
 * Helper: Update command timestamp for failsafe
*/
static void update_command_time(motor_controller_t* mc) {
    mc->last_command_time_ms = to_ms_since_boot(get_absolute_time());
    mc->failsafe_triggered = false;
}

void motor_controller_init(motor_controller_t* mc) {
    printf("Initializing motor controller...\n");

    // Initialize motors
    get_wheel_motor(&mc->motor_left, PIN_MOTOR_LEFT);
    get_wheel_motor(&mc->motor_right, PIN_MOTOR_RIGHT);
    get_weapon_motor(&mc->weapon, PIN_WEAPON);

    // Initialize state
    mc->left_speed = 0;
    mc->right_speed = 0;
    mc->weapon_speed = 0;
    mc->last_command_time_ms = to_ms_since_boot(get_absolute_time());
    mc->failsafe_triggered = false;

    motor_controller_stop_all(mc);
    sleep_ms(1000);

    printf("Motor controller ready\n");
}

void motor_controller_set(motor_t* m, int* cSpeed, int speed) {
    speed = deadband(speed);
    speed = clamp_int(speed, -MOTOR_MAX_SPEED, MOTOR_MAX_SPEED);

    motor_set_speed(m, speed);
    *cSpeed = speed;
}

void motor_controller_tank_drive(motor_controller_t* mc, int left, int right) {
    motor_controller_set(&mc->motor_left, &mc->left_speed, left);
    motor_controller_set(&mc->motor_right, &mc->right_speed, right);
    update_command_time(mc);
}


void motor_controller_stop_all(motor_controller_t* mc) {
    motor_stop(&mc->motor_left);
    motor_stop(&mc->motor_right);
    motor_stop(&mc->weapon);
    mc->left_speed = 0;
    mc->right_speed = 0;
    mc->weapon_speed = 0;
}

bool motor_controller_check_failsafe(motor_controller_t* mc) {
    if (!FAILSAFE_ENABLED) {
        return false;
    }

    uint32_t now = to_ms_since_boot(get_absolute_time());
    uint32_t elapsed = now - mc->last_command_time_ms;

    if (elapsed > FAILSAFE_TIMEOUT_MS) {
        if (!mc->failsafe_triggered) {
            // Only print if motors were actually running
            if (mc->left_speed != 0 || mc->right_speed != 0 || mc->weapon_speed != 0) {
                printf("Failsafe: motors stopped\n");
            }
            motor_controller_stop_all(mc);
            mc->failsafe_triggered = true;
        }
        return true;
    }

    return false;
}

void motor_controller_get_status(motor_controller_t* mc, int* left, int* right, int* weapon) {
    if (left) *left = mc->left_speed;
    if (right) *right = mc->right_speed;
    if (weapon) *weapon = mc->weapon_speed;
}
