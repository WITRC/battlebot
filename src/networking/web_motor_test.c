#include "web_motor_test.h"

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

typedef enum {
    WEB_TEST_MOTOR_LEFT = 0,
    WEB_TEST_MOTOR_RIGHT,
    WEB_TEST_MOTOR_WEAPON,
} web_test_motor_t;

typedef struct {
    bool active;
    web_test_motor_t motor;
    int power;
    uint32_t started_ms;
    uint32_t duration_ms;
} web_test_run_t;

static motor_controller_t* g_motors_ctrl = NULL;
static web_test_run_t g_test_run = {0};

static const char* web_test_motor_name(web_test_motor_t motor) {
    switch (motor) {
        case WEB_TEST_MOTOR_LEFT:
            return "left";
        case WEB_TEST_MOTOR_RIGHT:
            return "right";
        case WEB_TEST_MOTOR_WEAPON:
            return "weapon";
        default:
            return "unknown";
    }
}

static bool parse_web_test_motor(const char* value, web_test_motor_t* motor) {
    if (strcmp(value, "left") == 0) {
        *motor = WEB_TEST_MOTOR_LEFT;
        return true;
    }
    if (strcmp(value, "right") == 0) {
        *motor = WEB_TEST_MOTOR_RIGHT;
        return true;
    }
    if (strcmp(value, "weapon") == 0) {
        *motor = WEB_TEST_MOTOR_WEAPON;
        return true;
    }
    return false;
}

static uint32_t web_test_remaining_ms(uint32_t now_ms) {
    if (!g_test_run.active || g_test_run.duration_ms == 0) {
        return 0;
    }

    const uint32_t elapsed_ms = now_ms - g_test_run.started_ms;
    if (elapsed_ms >= g_test_run.duration_ms) {
        return 0;
    }

    return g_test_run.duration_ms - elapsed_ms;
}

static void apply_web_test_output(web_test_motor_t motor, int power) {
    switch (motor) {
        case WEB_TEST_MOTOR_LEFT:
            motor_controller_stop_all(g_motors_ctrl);
            motor_controller_set_motor((motor_t*)&g_motors_ctrl->motor_left, &g_motors_ctrl->left_speed, power);
            break;
        case WEB_TEST_MOTOR_RIGHT:
            motor_controller_stop_all(g_motors_ctrl);
            motor_controller_set_motor((motor_t*)&g_motors_ctrl->motor_right, &g_motors_ctrl->right_speed, power);
            break;
        case WEB_TEST_MOTOR_WEAPON:
            motor_controller_stop_all(g_motors_ctrl);
            motor_controller_set_motor((motor_t*)&g_motors_ctrl->weapon, &g_motors_ctrl->weapon_speed, power);
            break;
    }
}

static void clear_web_test_run(void) {
    g_test_run.active = false;
    g_test_run.power = 0;
    g_test_run.started_ms = 0;
    g_test_run.duration_ms = 0;
}

void web_motor_test_init(motor_controller_t* motors) {
    g_motors_ctrl = motors;
    clear_web_test_run();
}

void web_motor_test_deinit(void) {
    if (g_test_run.active) {
        web_motor_test_stop("web server stop");
    }

    g_motors_ctrl = NULL;
    clear_web_test_run();
}

bool web_motor_test_is_active(void) {
    return g_test_run.active;
}

bool web_motor_test_start(const char* motor_name, int power, uint32_t duration_ms) {
    web_test_motor_t motor = WEB_TEST_MOTOR_LEFT;

    if (g_motors_ctrl == NULL || !parse_web_test_motor(motor_name, &motor)) {
        return false;
    }

    if (g_test_run.active) {
        web_motor_test_stop("replaced by new web request");
    }

    g_test_run.active = true;
    g_test_run.motor = motor;
    g_test_run.power = power;
    g_test_run.started_ms = to_ms_since_boot(get_absolute_time());
    g_test_run.duration_ms = duration_ms;

    apply_web_test_output(motor, power);

    printf("Web Server: motor test start motor=%s power=%d duration_ms=%lu\n",
           web_test_motor_name(g_test_run.motor),
           g_test_run.power,
           (unsigned long)g_test_run.duration_ms);

    return true;
}

void web_motor_test_stop(const char* reason) {
    if (g_motors_ctrl != NULL) {
        motor_controller_stop_all(g_motors_ctrl);
    }

    if (!g_test_run.active) {
        return;
    }

    printf("Web Server: motor test stopped (%s)\n", reason);
    clear_web_test_run();
}

void web_motor_test_service(uint32_t now_ms) {
    if (g_test_run.active && g_test_run.duration_ms > 0 && web_test_remaining_ms(now_ms) == 0) {
        web_motor_test_stop("duration elapsed");
    }
}

void web_motor_test_snapshot(uint32_t now_ms, web_motor_test_status_t* status) {
    if (status == NULL) {
        return;
    }

    status->active = g_test_run.active;
    status->motor_name = g_test_run.active ? web_test_motor_name(g_test_run.motor) : "none";
    status->power = g_test_run.active ? g_test_run.power : 0;
    status->remaining_ms = g_test_run.active ? web_test_remaining_ms(now_ms) : 0;
}

bool web_motor_test_interrupt(void) {
    if (!g_test_run.active) {
        return false;
    }

    web_motor_test_stop("controller input");
    return true;
}
