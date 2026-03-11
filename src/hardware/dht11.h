#ifndef _DHT_H_
#define _DHT_H_

#include <pico/stdint.h>

typedef enum dht_model_t {
    DHT11
} dht_model_t;

/**
 * \brief DHT sensor.
 */
typedef struct dht_t {
    uint gpio_pin;
    float temperature;
    float humidity;
} dht_sensor_t;

void dht11_init(dht11_t *sensor, uint pin);
bool dht11_read(dht11_t *sensor);

#endif
