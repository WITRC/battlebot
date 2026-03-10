// =============================================================================
// Cooling Fan Driver for Pico 2 W
// Controls a DC fan using PWM on PIN_FAN (see config.h)
// =============================================================================

#ifndef FAN_H
#define FAN_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Initialize the cooling fan GPIO and PWM.
 * Must be called once at startup before using other fan functions.
 */
void fan_init(void);

/**
 * Set fan speed as a percentage (0-100).
 *
 * @param percent  Desired speed percentage (values are clamped to 0-100)
 */
void fan_set_speed_percent(int percent);

/**
 * Convenience helper: turn fan fully on (100% duty).
 */
void fan_on(void);

/**
 * Convenience helper: turn fan fully off (0% duty).
 */
void fan_off(void);

#endif // FAN_H

