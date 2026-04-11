/**
 * @file dht11.c
 * @brief DHT11 single-wire protocol implementation.
 *
 * Protocol:
 *   1. MCU pulls data line LOW for ≥18 ms (start signal)
 *   2. MCU releases line, DHT11 responds with 80 µs LOW + 80 µs HIGH
 *   3. DHT11 sends 40 bits: each bit is 50 µs LOW, then HIGH
 *      - HIGH ≈ 26–28 µs → bit 0
 *      - HIGH ≈ 70 µs    → bit 1
 *   4. Data: [humidity_int, humidity_dec, temp_int, temp_dec, checksum]
 */

#include "dht11.h"

#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "config.h"

#define DHT_TIMEOUT_US  1000  // Max wait for any single edge transition

/**
 * @brief Wait for the data pin to reach the expected level.
 * @return Duration in microseconds, or UINT32_MAX on timeout.
 */
static uint32_t wait_for_level(bool level) {
    uint32_t elapsed = 0;
    while (gpio_get(PIN_DHT11) != level) {
        if (elapsed++ >= DHT_TIMEOUT_US) {
            return UINT32_MAX;
        }
        busy_wait_us_32(1);
    }
    return elapsed;
}

/**
 * @brief Measure how long the pin stays at the given level.
 * @return Duration in microseconds, or UINT32_MAX on timeout.
 */
static uint32_t measure_pulse(bool level) {
    uint32_t elapsed = 0;
    while (gpio_get(PIN_DHT11) == level) {
        if (elapsed++ >= DHT_TIMEOUT_US) {
            return UINT32_MAX;
        }
        busy_wait_us_32(1);
    }
    return elapsed;
}

void dht11_init(void) {
    gpio_init(PIN_DHT11);
    gpio_set_dir(PIN_DHT11, GPIO_IN);
    gpio_pull_up(PIN_DHT11);

    printf("[DHT11] Initialized on GP%d\n", PIN_DHT11);
}

bool dht11_read(DHT11Data *data) {
    uint8_t bytes[5] = {0};

    // ── Start signal: pull LOW for 20 ms, then release ──
    gpio_set_dir(PIN_DHT11, GPIO_OUT);
    gpio_put(PIN_DHT11, 0);
    sleep_ms(20);
    gpio_put(PIN_DHT11, 1);
    busy_wait_us_32(30);
    gpio_set_dir(PIN_DHT11, GPIO_IN);

    // ── DHT11 response: ~80 µs LOW, then ~80 µs HIGH ──
    if (wait_for_level(false) == UINT32_MAX) {
        printf("[DHT11] No response (waiting for LOW)\n");
        return false;
    }
    if (measure_pulse(false) == UINT32_MAX) {
        printf("[DHT11] Timeout during response LOW\n");
        return false;
    }
    if (measure_pulse(true) == UINT32_MAX) {
        printf("[DHT11] Timeout during response HIGH\n");
        return false;
    }

    // ── Read 40 data bits ──
    for (int i = 0; i < 40; i++) {
        // Each bit starts with ~50 µs LOW
        if (measure_pulse(false) == UINT32_MAX) {
            printf("[DHT11] Timeout at bit %d (LOW phase)\n", i);
            return false;
        }

        // HIGH duration determines bit value
        uint32_t high_us = measure_pulse(true);
        if (high_us == UINT32_MAX) {
            printf("[DHT11] Timeout at bit %d (HIGH phase)\n", i);
            return false;
        }

        // > 40 µs HIGH = bit 1, otherwise bit 0
        bytes[i / 8] <<= 1;
        if (high_us > 40) {
            bytes[i / 8] |= 1;
        }
    }

    // ── Verify checksum ──
    uint8_t checksum = bytes[0] + bytes[1] + bytes[2] + bytes[3];
    if (checksum != bytes[4]) {
        printf("[DHT11] Checksum failed: got 0x%02X, expected 0x%02X\n",
               bytes[4], checksum);
        return false;
    }

    data->humidity    = bytes[0];
    data->temperature = bytes[2];
    return true;
}
