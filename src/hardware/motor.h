// =============================================================================
// ESC Motor Driver for Pico 2 W
// Controls brushless ESCs using 50Hz PWM (servo-style pulse widths)
// =============================================================================

#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include <stdbool.h>

#ifndef UNIT_TESTING
    #include "pico/stdlib.h"  // For uint and Pico types
#else
#endif

// PWM configuration for 50Hz operation
// Pico runs at 125MHz by default
// For 50Hz with good resolution: 125MHz / (wrap * divider) = 50Hz
// Using wrap=20000 and divider=125 gives exactly 50Hz with 1us resolution
#define PWM_WRAP     20000  // 20ms period = 50Hz
#define PWM_DIVIDER  150.0f // Clock divider

// Motor instance structure
typedef struct {
    uint gpio_pin;           // GPIO pin number
    uint slice_num;          // PWM slice number (derived from pin)
    uint channel;            // PWM channel (A or B)
    uint16_t min_us;         // Minimum pulse width (microseconds)
    uint16_t max_us;         // Maximum pulse width (microseconds)
    bool bidirectional;      // True if ESC supports reverse (uses mid_us as stop)
} motor_t;

#define WHEEL_MOTOR_MIN_US 1000
#define WHEEL_MOTOR_MAX_US 2000

#define WEAPON_MOTOR_MIN_US 1500
#define WEAPON_MOTOR_MAX_US 2000

/**
 * Initialize a motor on the specified GPIO pin.
 * Sets up PWM at 50Hz and starts with motor stopped.
 *
 * @param motor     Pointer to motor structure to initialize
 * @param gpio_pin  GPIO pin connected to ESC signal wire
 * @param min_us    Minimum pulse width in microseconds (default: 1000)
 * @param max_us    Maximum pulse width in microseconds (default: 2000)
 */

void get_wheel_motor(motor_t* m, uint pin);

void get_weapon_motor(motor_t* m, uint pin);

/**
 * Set speed as a value from -100 to 100.
 * For bidirectional ESCs: -100 = full reverse, 0 = stop, 100 = full forward
 * For unidirectional ESCs: Uses absolute value (0-100 range)
 *
 * @param m         Motor to control
 * @param speed         Speed value (-100 to 100)
 */
void motor_set_speed(motor_t* m, int speed);

/**
 * Stop motor immediately (sets to min_us for unidirectional, mid_us for bidirectional).
 */
void motor_stop(motor_t* motor);
#endif // MOTOR_H