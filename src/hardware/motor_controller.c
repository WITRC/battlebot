/**
 * @file motor_controller.c
 * @brief Motor controller implementation: tank drive, weapon control, failsafe.
 */

#include "motor_controller.h"
#include <stdio.h>

#include "motor.h"
#include "motor_bi.h"
#include "motor_omni.h"
#include "utility.h"
#include "pico/time.h"

/** @brief Refresh the last-command timestamp and clear any active failsafe. */
static void update_command_time(motor_controller_t* mc) {
    mc->last_command_time_ms = to_ms_since_boot(get_absolute_time());
    mc->failsafe_triggered = false;
}

void motor_controller_init(motor_controller_t* mc) {
    printf("Initializing motor controller...\n");

    motor_bi_ctor(&mc->motor_left, PIN_MOTOR_TRH_LEFT, PIN_MOTOR_REV_LEFT);
    motor_bi_ctor(&mc->motor_right, PIN_MOTOR_TRH_RIGHT, PIN_MOTOR_REV_RIGHT);
    motor_omni_ctor(&mc->weapon, PIN_WEAPON);

    motor_init((motor_t*)&mc->motor_left);
    motor_init((motor_t*)&mc->motor_right);
    motor_init((motor_t*)&mc->weapon);

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

    // Ramp toward target instead of jumping instantly
    int delta = speed - *cSpeed;
    int limit = (delta > 0) ? MOTOR_RAMP_UP : MOTOR_RAMP_DOWN;
    if (delta >  limit) delta =  limit;
    if (delta < -limit) delta = -limit;
    speed = *cSpeed + delta;

    motor_set_speed(m, speed);
    *cSpeed = speed;
}

void motor_controller_tank_drive(motor_controller_t* mc, int left, int right) {
    motor_controller_set((motor_t*)&mc->motor_left, &mc->left_speed, left);
    motor_controller_set((motor_t*)&mc->motor_right, &mc->right_speed, right);
    update_command_time(mc);
}

void motor_controller_weapon(motor_controller_t* mc, int weapon) {
    motor_controller_set((motor_t*)&mc->weapon, &mc->weapon_speed, weapon);
    update_command_time(mc);
}


void motor_controller_stop_all(motor_controller_t* mc) {
    motor_stop((motor_t*)&mc->motor_left);
    motor_stop((motor_t*)&mc->motor_right);
    motor_stop((motor_t*)&mc->weapon);
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
            return true;
        }
    }

    return false;
}

void motor_controller_get_status(motor_controller_t* mc, int* left, int* right, int* weapon) {
    if (left) *left = mc->left_speed;
    if (right) *right = mc->right_speed;
    if (weapon) *weapon = mc->weapon_speed;
}
