/**
 * @file motor_test_main.c
 * @brief Standalone motor test over USB serial.
 *
 * Plug the Pico into your laptop, flash motor_test.uf2, and connect with:
 *   screen /dev/tty.usbmodem* 115200
 *
 * Commands (type and press Enter):
 *   left  50 3     - run left motor at 50% forward for 3 seconds
 *   right -75 1.5  - run right motor at 75% reverse for 1.5 seconds
 *   weapon 80 5    - run weapon at 80% for 5 seconds
 *   left  100      - run at full speed indefinitely (until 's')
 *   s              - stop all motors
 *   q              - stop all and exit
 *   ?              - show help
 *
 * Speed range: -100 to +100 for drive motors, 0 to +100 for weapon.
 * Duration is optional; omit to hold until manually stopped.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "motor_bi.h"
#include "motor_omni.h"
#include "config.h"

/* -------------------------------------------------------------------------
 * Motor instances
 * ---------------------------------------------------------------------- */

static motor_bi_t  left_motor;
static motor_bi_t  right_motor;
static motor_omni_t weapon_motor;

static void init_all_motors(void) {
    motor_bi_ctor(&left_motor,   PIN_MOTOR_TRH_LEFT,  PIN_MOTOR_REV_LEFT);
    motor_bi_ctor(&right_motor,  PIN_MOTOR_TRH_RIGHT, PIN_MOTOR_REV_RIGHT);
    motor_omni_ctor(&weapon_motor, PIN_WEAPON);

    left_motor.base.v->init(&left_motor.base);
    right_motor.base.v->init(&right_motor.base);
    weapon_motor.base.v->init(&weapon_motor.base);
}

static void stop_all(void) {
    left_motor.base.v->stop(&left_motor.base);
    right_motor.base.v->stop(&right_motor.base);
    weapon_motor.base.v->stop(&weapon_motor.base);
}

/* -------------------------------------------------------------------------
 * Command parsing helpers
 * ---------------------------------------------------------------------- */

/** Trim leading/trailing whitespace in-place, return pointer into buf. */
static char *trim(char *s) {
    while (*s == ' ' || *s == '\t') s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n'))
        *end-- = '\0';
    return s;
}

static void print_help(void) {
    printf("\nCommands:\n");
    printf("  <motor> <speed> [duration]\n");
    printf("    motor    : left | right | weapon\n");
    printf("    speed    : -100..100 (drive), 0..100 (weapon)\n");
    printf("    duration : seconds to run, then auto-stop (optional)\n");
    printf("  s / stop   : stop all motors immediately\n");
    printf("  q / quit   : stop all and exit\n");
    printf("  ?          : show this help\n\n");
}

/* -------------------------------------------------------------------------
 * Main
 * ---------------------------------------------------------------------- */

int main(void) {
    stdio_init_all();

    /* Wait for USB serial to enumerate on the host side */
    sleep_ms(2000);

    printf("\n");
    printf("====================================\n");
    printf("  %s — Motor Test\n", ROBOT_NAME);
    printf("====================================\n");
    printf("Initializing motors...\n");

    init_all_motors();

    printf("Arming ESCs (holding neutral for 5 s)...\n");
    sleep_ms(5000);
    printf("Ready.\n");
    print_help();

    char line[64];
    bool running = true;

    while (running) {
        printf("cmd> ");
        fflush(stdout);

        /* Read a line from USB serial */
        int idx = 0;
        while (idx < (int)sizeof(line) - 1) {
            int c = getchar();
            if (c == '\r' || c == '\n') {
                line[idx] = '\0';
                /* Echo newline so terminal looks right */
                putchar('\n');
                break;
            }
            if (c == '\b' || c == 127) {
                if (idx > 0) { idx--; printf("\b \b"); fflush(stdout); }
                continue;
            }
            if (c >= 32 && c < 127) {
                line[idx++] = (char)c;
                putchar(c);
                fflush(stdout);
            }
        }
        line[idx] = '\0';

        char *cmd = trim(line);
        if (cmd[0] == '\0') continue;

        /* --- quit --- */
        if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0) {
            running = false;
            break;
        }

        /* --- stop --- */
        if (strcmp(cmd, "s") == 0 || strcmp(cmd, "stop") == 0) {
            stop_all();
            printf("All motors stopped.\n");
            continue;
        }

        /* --- help --- */
        if (strcmp(cmd, "?") == 0) {
            print_help();
            continue;
        }

        /* --- motor speed [duration] --- */
        char motor_name[16] = {0};
        int  speed = 0;
        float duration = -1.0f;  /* -1 means hold indefinitely */

        int parsed = sscanf(cmd, "%15s %d %f", motor_name, &speed, &duration);
        if (parsed < 2) {
            printf("Invalid command. Type ? for help.\n");
            continue;
        }

        /* Resolve motor */
        motor_t *m = NULL;
        bool is_weapon = false;

        if (strcmp(motor_name, "left") == 0) {
            m = &left_motor.base;
        } else if (strcmp(motor_name, "right") == 0) {
            m = &right_motor.base;
        } else if (strcmp(motor_name, "weapon") == 0) {
            m = &weapon_motor.base;
            is_weapon = true;
        } else {
            printf("Unknown motor '%s'. Use: left | right | weapon\n", motor_name);
            continue;
        }

        /* Clamp speed */
        if (is_weapon) {
            if (speed < 0) speed = 0;
            if (speed > 100) speed = 100;
        } else {
            if (speed < -100) speed = -100;
            if (speed > 100) speed = 100;
        }

        /* Print what we're doing */
        const char *dir = "";
        if (!is_weapon) {
            dir = (speed > 0) ? "FORWARD" : (speed < 0) ? "REVERSE" : "NEUTRAL";
        }

        if (duration > 0.0f) {
            if (is_weapon)
                printf("%s: %d%% for %.1f s\n", motor_name, speed, duration);
            else
                printf("%s: %s %d%% for %.1f s\n", motor_name, dir, abs(speed), duration);
        } else {
            if (is_weapon)
                printf("%s: %d%% (hold — type 's' to stop)\n", motor_name, speed);
            else
                printf("%s: %s %d%% (hold — type 's' to stop)\n", motor_name, dir, abs(speed));
        }

        /* Drive the motor */
        m->v->set_speed(m, speed);

        if (duration > 0.0f) {
            sleep_ms((uint32_t)(duration * 1000.0f));
            m->v->stop(m);
            printf("Done.\n");
        }
    }

    printf("\nStopping all motors...\n");
    stop_all();
    printf("Goodbye.\n");
    return 0;
}
