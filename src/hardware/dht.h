#ifndef _DHT_H_
#define _DHT_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum dht_model_t {
    DHT11
} dht_model_t;

/**
 * \brief DHT sensor.
 */
typedef struct dht_t {
    uint8_t gpio_pin;
    dht_type_t type;
    float temperature;
    float humidity;
} dht_sensor_t;

bool dht_init(dht_sensor_t* sensor, uinit8_t pin, dht_type_t dhtType);
init8_t dht_read_data(dht_sensor_t* sensor);

#endif
