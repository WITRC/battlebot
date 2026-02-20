// =============================================================================
// Monster Book of Monsters - Central Configuration
// All hardware pins, settings, and tunables in one place
// =============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include "secrets.h"  // Contains SECRET_WIFI_PASSWORD (gitignored)

// =============================================================================
// ROBOT IDENTITY
// =============================================================================

#define ROBOT_NAME  "Monster Book of Monsters"

// =============================================================================
// HARDWARE PIN ASSIGNMENTS
// =============================================================================

// ESC Motor Pins (PWM signal to ESC)
#define PIN_MOTOR_TRH_LEFT         0   // GP0 - Left drive motor
#define PIN_MOTOR_REV_LEFT         1   // GP0 - Left drive motor
#define PIN_MOTOR_TRH_RIGHT         2   // GP0 - Left drive motor
#define PIN_MOTOR_REV_RIGHT        3   // GP0 - Left drive motor
#define PIN_WEAPON             4   // GP2 - Weapon motor

// Analog Inputs
#define PIN_BATTERY_ADC  26  // ADC0 - Battery voltage divider

// Status LED is handled by CYW43 (CYW43_WL_GPIO_LED_PIN)

// =============================================================================
// MOTOR SETTINGS
// =============================================================================

// ESC PWM Configuration (50Hz servo-style)
#define MOTOR_PWM_FREQ      50      // Hz

// Motor Behavior
#define MOTOR_MAX_SPEED     100     // Maximum speed percentage
#define MOTOR_DEADBAND      10      // Ignore inputs below this %

// =============================================================================
// SAFETY SETTINGS
// =============================================================================

#define FAILSAFE_ENABLED    true
#define FAILSAFE_TIMEOUT_MS 2000    // Time without commands before triggering failsafe

// Safety limits (absolute min/max to prevent ESC damage)
#define ESC_ABS_MIN_US      900
#define ESC_ABS_MAX_US      2100

// Low battery cutoff (disable if no battery sensor connected)
#define ENABLE_LOW_BATTERY_CUTOFF  false

// =============================================================================
// BATTERY SETTINGS (3S LiPo)
// =============================================================================

#define BATTERY_CELLS           3
#define BATTERY_MIN_VOLTAGE     10.0f   // Emergency stop (3.33V/cell)
#define BATTERY_LOW_VOLTAGE     10.8f   // Warning (3.6V/cell)
#define BATTERY_CRITICAL_VOLTAGE 10.2f  // Critical (3.4V/cell)
#define BATTERY_MAX_VOLTAGE     12.6f   // Fully charged (4.2V/cell)
#define BATTERY_ADC_RATIO       5.7f    // Voltage divider ratio

// =============================================================================
// Wi-Fi SETTINGS
// =============================================================================

#define WIFI_AP_SSID      "Monster Book of Monsters"
#define WIFI_AP_PASSWORD  SECRET_WIFI_PASSWORD  // From secrets.h
#define WIFI_AP_CHANNEL   11

// AP IP Configuration
#define WIFI_AP_IP        "192.168.4.1"
#define WIFI_AP_NETMASK   "255.255.255.0"
#define WIFI_AP_GATEWAY   "192.168.4.1"

// =============================================================================
// WEB SERVER SETTINGS
// =============================================================================

#define WEB_SERVER_PORT   80

// =============================================================================
// CONTROLLER MAPPING
// =============================================================================

// Xbox controller stick ranges
#define STICK_MIN    -512
#define STICK_MAX     511
#define TRIGGER_MAX   1023

// Invert controls if needed (-1 to invert, 1 for normal)
#define THROTTLE_INVERT  -1  // Push forward = forward (Y axis inverted)
#define TURN_INVERT       1  // Push right = turn right

// =============================================================================
// DEBUG SETTINGS
// =============================================================================

#define VERBOSE_LOGGING      false

#endif // CONFIG_H
