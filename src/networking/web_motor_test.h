/**
 * @file web_motor_test.h
 * @brief Web-triggered one-shot motor test control.
 *
 * Owns the temporary motor-test state used by the dashboard and exposes a
 * small API for starting, stopping, servicing, and observing that test run.
 */
#ifndef WEB_MOTOR_TEST_H
#define WEB_MOTOR_TEST_H

#include <stdbool.h>
#include <stdint.h>

#include "motor_controller.h"

/** Snapshot of the current web motor-test state. */
typedef struct {
    bool active;             /**< True when a web-started test is running. */
    const char* motor_name;  /**< Active motor name, or "none". */
    int power;               /**< Current commanded power percent. */
    uint32_t remaining_ms;   /**< Remaining time, or 0 for hold/manual stop. */
} web_motor_test_status_t;

/** Initialize the web motor-test module with the shared motor controller. */
void web_motor_test_init(motor_controller_t* motors);

/** Stop any active web test and detach the motor controller. */
void web_motor_test_deinit(void);

/** @brief Return true if a web-started test is currently active. */
bool web_motor_test_is_active(void);

/**
 * @brief Start a new one-shot motor test.
 * @param motor_name   Target motor name: left, right, or weapon.
 * @param power        Commanded power percent.
 * @param duration_ms  Duration in milliseconds, or 0 to hold until stopped.
 * @return true on success, false if the request is invalid.
 */
bool web_motor_test_start(const char* motor_name, int power, uint32_t duration_ms);

/** @brief Stop the current web motor test and stop all motors. */
void web_motor_test_stop(const char* reason);

/**
 * Check whether the active timed test has expired and stop it if needed.
 * Call this periodically from the web server timer path.
 */
void web_motor_test_service(uint32_t now_ms);

/**
 * @brief Fill @p status with the current web motor-test snapshot.
 * @param now_ms  Current monotonic time in milliseconds.
 * @param status  Output snapshot to populate.
 */
void web_motor_test_snapshot(uint32_t now_ms, web_motor_test_status_t* status);

/** @brief Stop the active web motor test due to external control input. */
bool web_motor_test_interrupt(void);

#endif // WEB_MOTOR_TEST_H
