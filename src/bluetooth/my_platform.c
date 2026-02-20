#include <string.h>
#include <stdio.h>

#include <pico/stdlib.h>      // For to_ms_since_boot
#include <pico/cyw43_arch.h>  // For controlling the onboard LED
#include <pico/time.h>        // For repeating timer
#include <uni.h>              // Bluepad32 main header

#include "config.h"           // Central configuration
#include "sdkconfig.h"        // Bluepad32 configuration
#include "motor_controller.h" // Motor control
#include "utility.h"
#include "web_server.h"       // HTTP dashboard
#include "wifi_ap.h"

// Sanity check - Pico W requires custom platform mode
#ifndef CONFIG_BLUEPAD32_PLATFORM_CUSTOM
#error "Pico W must use BLUEPAD32_PLATFORM_CUSTOM"
#endif

// =============================================================================
// GLOBAL STATE
// =============================================================================
// platform states
static motor_controller_t motor_ctrl;
static bool xbox_pressed = false;
static bool prev_xbox_pressed = false;
// =============================================================================
// CONTROL MAPPING
// =============================================================================


/**
 * left stick Y = left motor, right stick Y = right motor, right trigger = weapon speed , Xbox button (system button) → emergency stop
 */
static void process_controller_input(uni_gamepad_t *gp) {
    if (motor_ctrl.state == initializing) {
        printf("Cannot process input while initializing");
        return;
    }

    // === EMERGENCY STOP ===
    xbox_pressed = (gp->misc_buttons & MISC_BUTTON_SYSTEM) != 0;

    if (xbox_pressed && !prev_xbox_pressed) {
        if (motor_ctrl.state == active) {
            motor_ctrl.state = stopped;
            printf("EMERGENCY STOP ACTIVATED!\n");
            motor_controller_stop_all(&motor_ctrl);
        } else if (motor_ctrl.state == stopped) {
            printf("Resuming normal operation...\n");
            motor_ctrl.state = active;
        }
    }

    cyw43_arch_poll();
    if (motor_controller_check_failsafe(&motor_ctrl)) {
        //todo: doesn't work
        return;
    }


    prev_xbox_pressed = xbox_pressed;

    if (motor_ctrl.state == stopped) {
        printf(">>> Controller input received but currently stopped. Press Xbox button to resume.\n");
        return;
    }

    // === DRIVE CONTROL ===
    int left_speed = map_range_int(gp->axis_y, STICK_MIN, STICK_MAX, -100, 100) * THROTTLE_INVERT;
    int right_speed = map_range_int(gp->axis_ry, STICK_MIN, STICK_MAX, -100, 100) * THROTTLE_INVERT;
    motor_controller_tank_drive(&motor_ctrl, left_speed, right_speed);

    // === WEAPON CONTROL ===
    int weapon_speed = map_range_int(gp->throttle, 0, TRIGGER_MAX, 0, 100);
    motor_controller_weapon(&motor_ctrl, weapon_speed);
}

// =============================================================================
// PLATFORM CALLBACKS
// =============================================================================

/**
 * Called once when Bluepad32 initializes.
 */
static void my_platform_init(int argc, const char **argv) {
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    printf("Monster Book of Monsters - Controller initialized\n");
}

/**
 * Called when Bluetooth stack is fully initialized and ready.
 */
static void my_platform_on_init_complete(void) {
    printf("\n");
    printf("==================================================\n");
    printf("  %s - Ready!\n", ROBOT_NAME);
    printf("==================================================\n");
    printf("\n");

    motor_controller_init(&motor_ctrl);
    motor_ctrl.state = stopped; // Start in "off" state until controller is connected

    // Initialize web server (Wi-Fi AP already started in main.c)
    wifi_ap_init();

    printf("web_server_init...\n");
    if (!web_server_init(&motor_ctrl)) {
        printf("FATAL: Failed to initialize web server!\n");
        return;
    }

    printf("\n");
    printf("Controls (Tank Drive):\n");
    printf("  Left Stick Y  : Left motor\n");
    printf("  Right Stick Y : Right motor\n");
    printf("  Right Trigger : Weapon speed\n");
    printf("  Xbox Button   : Emergency stop\n");
    printf("\n");

    printf("\n");
    printf("Waiting for Xbox controller...\n");
    printf("(Turn on controller or hold pair button)\n");
    printf("\n");

    // Start scanning for controllers
    uni_bt_start_scanning_and_autoconnect_unsafe();

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0); // LED off = waiting for controller
}

/**
 * Called when a Bluetooth device is discovered.
 * Filter to only accept Xbox controllers.
 */
//todo: replace with specific device UUID (AC:8E:BD:6C:6D:CC) (not sure)
static uni_error_t my_platform_on_device_discovered(bd_addr_t addr, const char *name, uint16_t cod, uint8_t rssi) {
    if (name != NULL && strstr(name, "Xbox") != NULL) {
        // filter by "Xbox" in name
        printf("Xbox controller found: %s (RSSI: %d)\n", name, rssi);
        return UNI_ERROR_SUCCESS;
    }

    if ((cod & 0x050C) == 0x0508) {
        // filter by Class of Device (Xbox BLE may have empty name initially)
        printf("Gamepad found (COD: 0x%04x, RSSI: %d)\n", cod, rssi);
        return UNI_ERROR_SUCCESS;
    }

    return UNI_ERROR_IGNORE_DEVICE; // Ignore everything else
}

/**
 * Called when a controller has connected.
 */
static void my_platform_on_device_connected(uni_hid_device_t *d) {
    printf("Controller connected!\n");
    motor_ctrl.state = stopped; // Set to "on" state when controller connects
    uni_bt_stop_scanning_safe();
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); // LED on = controller connected
}


static void my_platform_on_device_disconnected(uni_hid_device_t *d) {
    printf("\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("!!!     CONTROLLER DISCONNECTED - E-STOP      !!!\n");
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    // SAFETY: Emergency stop all motors immediately
    if (motor_ctrl.state != initializing) {
        printf(">>> Stopping all motors...\n");
        motor_controller_stop_all(&motor_ctrl);
        motor_ctrl.state = stopped; // Set to "off" state until controller reconnects
        printf(">>> All motors stopped, weapon disarmed\n");
    }

    // LED off = no controller
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    // Resume scanning for reconnection
    printf("\nScanning for reconnect...\n");
    uni_bt_start_scanning_and_autoconnect_safe();
}

static uni_error_t my_platform_on_device_ready(uni_hid_device_t *d) {
    printf("\n");
    printf("*** Controller ready - DRIVE ENABLED ***\n");
    printf("(Weapon ready - use right trigger)\n");
    printf("\n");
    return UNI_ERROR_SUCCESS;
}

static const uni_property_t *my_platform_get_property(uni_property_idx_t idx) {
    ARG_UNUSED(idx);
    return NULL;
}

static void my_platform_on_oob_event(uni_platform_oob_event_t event, void *data) {
    if (event == UNI_PLATFORM_OOB_BLUETOOTH_ENABLED) {
        printf("Bluetooth scanning: %s\n", (bool) (data) ? "on" : "off");
    }
}

// =============================================================================
// INPUT DISPLAY (for debugging)
// =============================================================================

static void print_buttons(uint32_t buttons, uint8_t misc_buttons) {
    printf("Buttons: ");

    if (buttons == 0 && misc_buttons == 0) {
        printf("none       ");
    } else {
        if (buttons & BUTTON_A) printf("A ");
        if (buttons & BUTTON_B) printf("B ");
        if (buttons & BUTTON_X) printf("X ");
        if (buttons & BUTTON_Y) printf("Y ");
        if (buttons & BUTTON_SHOULDER_L) printf("LB ");
        if (buttons & BUTTON_SHOULDER_R) printf("RB ");
        if (buttons & BUTTON_TRIGGER_L) printf("LT ");
        if (buttons & BUTTON_TRIGGER_R) printf("RT ");
        if (buttons & BUTTON_THUMB_L) printf("LS ");
        if (buttons & BUTTON_THUMB_R) printf("RS ");
        if (misc_buttons & MISC_BUTTON_START) printf("START ");
        if (misc_buttons & MISC_BUTTON_SELECT) printf("SELECT ");
        if (misc_buttons & MISC_BUTTON_SYSTEM) printf("XBOX ");
    }
}

static const char *dpad_to_string(uint8_t dpad) {
    switch (dpad) {
        case 0: return "none";
        case DPAD_UP: return "UP";
        case DPAD_DOWN: return "DOWN";
        case DPAD_RIGHT: return "RIGHT";
        case DPAD_LEFT: return "LEFT";
        case (DPAD_UP | DPAD_RIGHT): return "UP+RIGHT";
        case (DPAD_DOWN | DPAD_RIGHT): return "DOWN+RIGHT";
        case (DPAD_UP | DPAD_LEFT): return "UP+LEFT";
        case (DPAD_DOWN | DPAD_LEFT): return "DOWN+LEFT";
        default: return "?";
    }
}

/**
 * Called every time controller sends new input data.
 * This is the main control loop for the robot!
 */
static void my_platform_on_controller_data(uni_hid_device_t *d, uni_controller_t *ctl) {
    static uni_controller_t prev = {0};

    // Only process if something changed
    if (memcmp(&prev, ctl, sizeof(*ctl)) == 0) {
        motor_controller_check_failsafe(&motor_ctrl);
        return;
    }

    prev = *ctl;

    if (ctl->klass == UNI_CONTROLLER_CLASS_GAMEPAD) {
        uni_gamepad_t *gp = &ctl->gamepad;

        process_controller_input(gp);

        // === DEBUG OUTPUT ===
        printf("Motors: L=%+4d%% R=%+4d%% W=%3d%% | ",
               motor_ctrl.left_speed, motor_ctrl.right_speed, motor_ctrl.weapon_speed);
        print_buttons(gp->buttons, gp->misc_buttons);
        printf("| DPad: %-10s", dpad_to_string(gp->dpad));
        printf("| Sticks: (%+4d,%+4d) (%+4d,%+4d)",
               gp->axis_x, gp->axis_y, gp->axis_rx, gp->axis_ry);
        printf("| Trig: %4d %4d | State %1d\n", gp->brake, gp->throttle, motor_ctrl.state);
    }
}

// =============================================================================
// PLATFORM REGISTRATION
// =============================================================================

struct uni_platform *get_my_platform(void) {
    static struct uni_platform plat = {
        .name = "Monster Book of Monsters",
        .init = my_platform_init,
        .on_init_complete = my_platform_on_init_complete,
        .on_device_discovered = my_platform_on_device_discovered,
        .on_device_connected = my_platform_on_device_connected,
        .on_device_disconnected = my_platform_on_device_disconnected,
        .on_device_ready = my_platform_on_device_ready,
        .on_controller_data = my_platform_on_controller_data,
        .on_oob_event = my_platform_on_oob_event,
        .get_property = my_platform_get_property,
    };

    return &plat;
}
