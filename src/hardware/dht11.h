/**
 * @file dht11.h
 * @brief DHT11 temperature and humidity sensor driver (single-wire protocol).
 */
#ifndef DHT11_H
#define DHT11_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t humidity;       // Relative humidity (0–99 %)
    uint8_t temperature;    // Temperature (0–50 °C)
} DHT11Data;

/** @brief Initialize the DHT11 GPIO pin. Call once at startup. */
void dht11_init(void);

/**
 * @brief Read temperature and humidity from the sensor.
 *
 * The DHT11 can only be polled once every ~2 seconds.
 * Returns true if a valid reading was obtained.
 */
bool dht11_read(DHT11Data *data);

#endif // DHT11_H
