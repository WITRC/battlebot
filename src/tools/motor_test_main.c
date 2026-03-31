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

/** Instant brake — calls the motor's stop() vtable entry. */
static void kill_all(void) {
    left_motor.base.v->stop(&left_motor.base);
    right_motor.base.v->stop(&right_motor.base);
    weapon_motor.base.v->stop(&weapon_motor.base);
}

/**
 * Graceful ramp-down for a single motor.
 * Steps at MOTOR_RAMP_DOWN per 10 ms (matching the ~100 Hz main-code rate).
 * @param m      Motor to ramp.
 * @param speed  Current speed (sign-aware; weapon pass positive value).
 */
static void ramp_stop(motor_t *m, int speed) {
    while (speed != 0) {
        int step = (speed > 0) ? -MOTOR_RAMP_DOWN : MOTOR_RAMP_DOWN;
        speed += step;
        if ((step < 0 && speed < 0) || (step > 0 && speed > 0))
            speed = 0;
        m->v->set_speed(m, speed);
        sleep_ms(10);
    }
}

/** Graceful ramp-down then set_speed(0) on all motors (assumed already near 0). */
static void stop_all_graceful(motor_t *active_motor, int active_speed) {
    ramp_stop(active_motor, active_speed);
    /* Other motors should already be at 0; set_speed(0) to confirm. */
    left_motor.base.v->set_speed(&left_motor.base, 0);
    right_motor.base.v->set_speed(&right_motor.base, 0);
    weapon_motor.base.v->set_speed(&weapon_motor.base, 0);
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
    printf("  s / stop   : graceful ramp down to stop\n");
    printf("  k / c      : instant kill (hard brake)\n");
    printf("  q / quit   : kill and exit\n");
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
    /* Give ESCs time to power on and complete their own startup sequence
     * before we start sending PWM signals. The main code gets this for free
     * because BT init takes ~5-10s. */
    printf("Waiting for ESCs to power on (5 s)...\n");
    sleep_ms(5000);

    printf("Initializing motors...\n");
    init_all_motors();

    printf("Arming ESCs (holding neutral for 3 s)...\n");
    sleep_ms(3000);
    printf("Ready.\n");
    print_help();

    char line[64];
    bool running = true;
    motor_t *active_motor = NULL;
    int      active_speed = 0;

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

        /* --- graceful stop --- */
        if (strcmp(cmd, "s") == 0 || strcmp(cmd, "stop") == 0) {
            if (active_motor && active_speed != 0) {
                printf("Ramping down...\n");
                ramp_stop(active_motor, active_speed);
                active_speed = 0;
            }
            /* Zero out everything else */
            left_motor.base.v->set_speed(&left_motor.base, 0);
            right_motor.base.v->set_speed(&right_motor.base, 0);
            weapon_motor.base.v->set_speed(&weapon_motor.base, 0);
            printf("Stopped.\n");
            continue;
        }

        /* --- instant kill --- */
        if (strcmp(cmd, "k") == 0 || strcmp(cmd, "c") == 0) {
            kill_all();
            active_speed = 0;
            printf("Killed.\n");
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
                printf("%s: %d%% (hold — s to stop, k/c to kill)\n", motor_name, speed);
            else
                printf("%s: %s %d%% (hold — s to stop, k/c to kill)\n", motor_name, dir, abs(speed));
        }

        /* Drive the motor */
        m->v->set_speed(m, speed);
        active_motor = m;
        active_speed = speed;

        if (duration > 0.0f) {
            uint32_t remaining_ms = (uint32_t)(duration * 1000.0f);
            uint32_t last_printed_s = remaining_ms / 1000 + 1;
            bool did_kill = false;
            bool did_stop = false;
            char ibuf[8];
            int  ilen = 0;

            printf("  (type s+Enter to stop, k+Enter to kill)\n");

            while (remaining_ms > 0) {
                uint32_t slice = remaining_ms < 10 ? remaining_ms : 10;
                remaining_ms -= slice;

                /* Print countdown once per second */
                uint32_t secs_left = (remaining_ms + 999) / 1000;
                if (secs_left < last_printed_s) {
                    last_printed_s = secs_left;
                    printf("\r  %u s remaining...  ", (unsigned)secs_left);
                    fflush(stdout);
                }

                /* Drain all available input bytes without blocking */
                int c;
                while ((c = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT) {
                    if (c == '\r' || c == '\n') {
                        /* Trim and check the accumulated command */
                        while (ilen > 0 && (ibuf[ilen-1] == ' ' || ibuf[ilen-1] == '\t'))
                            ilen--;
                        ibuf[ilen] = '\0';
                        if (ibuf[0] == 'k' || ibuf[0] == 'c') did_kill = true;
                        if (ibuf[0] == 's')                    did_stop = true;
                        ilen = 0;
                    } else if (ilen < (int)sizeof(ibuf) - 1) {
                        ibuf[ilen++] = (char)c;
                    }
                    if (did_kill || did_stop) break;
                }

                sleep_ms(slice);

                if (did_kill || did_stop) break;
            }

            if (did_kill) {
                m->v->stop(m);
                active_speed = 0;
                printf("\nKilled.\n");
            } else if (did_stop) {
                printf("\nRamping down...\n");
                ramp_stop(m, active_speed);
                active_speed = 0;
                printf("Stopped.\n");
            } else {
                m->v->set_speed(m, 0);
                active_speed = 0;
                printf("\nDone.\n");
            }
        }
    }

    printf("\nStopping all motors...\n");
    kill_all();
    printf("Goodbye.\n");
    return 0;
}
