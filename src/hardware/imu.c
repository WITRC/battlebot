#include "imu.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "config.h"

static IMUData sensor_data = {0};

static bool imu_uart_readable(void) {
    return uart_is_readable(imu_uart);
}

static uint8_t imu_uart_read_byte(void) {
    return uart_getc(imu_uart);
}

static bool verify_checksum(const uint8_t *packet) {
    uint16_t sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += packet[i];
    }
    return ((sum & 0xFFu) == packet[10]);
}

static int16_t le_i16(const uint8_t *p) {
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static bool parse_packet(const uint8_t *packet) {
    if (packet == NULL) {
        return false;
    }

    if (packet[0] != packet_start) {
        return false;
    }

    if (!verify_checksum(packet)) {
        return false;
    }

    int16_t v1 = le_i16(&packet[2]);
    int16_t v2 = le_i16(&packet[4]);
    int16_t v3 = le_i16(&packet[6]);
    int16_t v4 = le_i16(&packet[8]);
    (void)v4;

    switch (packet[1]) {
        case packet_accel:
            sensor_data.accel_x = (float)v1 * accel_scale;
            sensor_data.accel_y = (float)v2 * accel_scale;
            sensor_data.accel_z = (float)v3 * accel_scale;
            break;

        case packet_gyro:
            sensor_data.gyro_x = (float)v1 * gyro_scale;
            sensor_data.gyro_y = (float)v2 * gyro_scale;
            sensor_data.gyro_z = (float)v3 * gyro_scale;
            break;

        case packet_angle:
            sensor_data.roll  = (float)v1 * angle_scale;
            sensor_data.pitch = (float)v2 * angle_scale;
            sensor_data.yaw   = (float)v3 * angle_scale;
            break;

        case packet_mag:
            sensor_data.mag_x = v1;
            sensor_data.mag_y = v2;
            sensor_data.mag_z = v3;
            break;

        default:
            break;
    }

    return true;
}

void imu_init(void) {
    uart_init(imu_uart, imu_baud);

    gpio_set_function(imu_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(imu_rx_pin, GPIO_FUNC_UART);
    gpio_pull_up(imu_rx_pin);

    uart_set_format(imu_uart, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(imu_uart, true);

    sleep_ms(200);

    printf("[IMU] UART0 initialized at %d baud on TX=%d RX=%d\n",
           imu_baud, imu_tx_pin, imu_rx_pin);
}

bool imu_update(void) {
    static uint8_t packet[packet_length];
    static int index = 0;
    bool got_packet = false;

    while (imu_uart_readable()) {
        uint8_t b = imu_uart_read_byte();
        if (index == 0) {
            if (b != packet_start) {
                continue;
            }
            packet[index++] = b;
            continue;
        }

        packet[index++] = b;

        if (index >= packet_length) {
            if (parse_packet(packet)) {
                got_packet = true;
            }
            index = 0;
        }
    }

    return got_packet;
}

IMUData imu_get_data(void) {
    return sensor_data;
}
