// =============================================================================
// Motor Controller for BattleBot
// High-level control: tank/arcade drive, weapon, failsafe, arming
// =============================================================================

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "motor.h"
#include "motor_bi.h"
#include "motor_omni.h"

// =============================================================================
// MOTOR CONTROLLER API
// =============================================================================

// Motor controller instance (initialized when Bluetooth is ready)
typedef enum {initializing, active, stopped} p_state;

// Motor controller state
typedef struct {
    motor_bi_t motor_left;
    motor_bi_t motor_right;
    motor_omni_t weapon;

    // Current speeds (-100 to 100 for drive, 0 to 100 for weapon)
    int left_speed;
    int right_speed;
    int weapon_speed;

    p_state state;


    // Failsafe tracking
    uint32_t last_command_time_ms;
    bool failsafe_triggered;
} motor_controller_t;

/**
 * Initialize the motor controller.
 * Sets up PWM for all motors and starts in stopped/disarmed state.
 */
void motor_controller_init(motor_controller_t* mc);

void motor_controller_tank_drive(motor_controller_t* mc, int left, int right);
void motor_controller_weapon(motor_controller_t* mc, int weapon);

void motor_controller_set(motor_t* m, int* cSpeed, int speed);

/**
 * Emergency stop - all motors to zero, weapon disarmed.
 */
void motor_controller_stop_all(motor_controller_t* mc);

/**
 * Check and apply failsafe if no commands received recently.
 * Call this periodically (e.g., every main loop iteration).
 * @return true if failsafe was triggered
 */
bool motor_controller_check_failsafe(motor_controller_t* mc);

void motor_controller_get_status(motor_controller_t* mc, int* left, int* right, int* weapon);

#endif // MOTOR_CONTROLLER_H