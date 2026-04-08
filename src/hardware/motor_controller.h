/**
 * @file motor_controller.h
 * @brief High-level BattleBot motor controller.
 *
 * Provides tank drive, weapon control, failsafe, and arming logic on top of
 * the low-level motor drivers.
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "motor.h"
#include "motor_bi.h"
#include "motor_omni.h"

/** Operational state of the motor controller. */
typedef enum { initializing, active, stopped } p_state;

/** Aggregate state for all drive and weapon motors. */
typedef struct {
    motor_bi_t motor_left;   /**< Left drive motor. */
    motor_bi_t motor_right;  /**< Right drive motor. */
    motor_omni_t weapon;     /**< Weapon/spinner motor. */

    int left_speed;          /**< Current left speed  [-100, 100]. */
    int right_speed;         /**< Current right speed [-100, 100]. */
    int weapon_speed;        /**< Current weapon speed [0, 100]. */

    p_state state;

    uint32_t last_command_time_ms; /**< Timestamp of last received command (ms). */
    bool failsafe_triggered;       /**< True when failsafe has fired. */
} motor_controller_t;

/**
 * Initialize the motor controller.
 * Sets up PWM for all motors and starts in stopped/disarmed state.
 */
void motor_controller_init(motor_controller_t* mc);

/**
 * @brief Set left/right drive speeds for tank-style steering.
 * @param left   Left-side speed  [-100, 100].
 * @param right  Right-side speed [-100, 100].
 */
void motor_controller_tank_drive(motor_controller_t* mc, int left, int right);

/** @brief Set weapon motor speed. @param weapon Speed [0, 100]. */
void motor_controller_weapon(motor_controller_t* mc, int weapon);

/**
 * @brief Update a single motor's speed, applying deadband/clamping, directly without interpolation
 * @param cSpeed  Pointer to the cached speed value to update.
 * @param speed   Desired speed [-100, 100].
 */
void motor_controller_set_motor(motor_t* m, int* cSpeed, int speed);

/**
 * @brief Update a single motor's speed applying deadband/clamping, using simple interpolation
 * @param cSpeed  Pointer to the cached speed value to update.
 * @param speed   Desired speed [-100, 100].
 */
void motor_controller_update_motor(motor_t* m, int* cSpeed, int speed);

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

/** @brief Read back the current cached speeds (out-params may be NULL). */
void motor_controller_get_status(motor_controller_t* mc, int* left, int* right, int* weapon);

#endif // MOTOR_CONTROLLER_H