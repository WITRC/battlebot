#include "dht11.h"
#include "hardware/gpio.h"

void dht11_init(dht11_t *sensor, uint pin) {
    sensor->pin = pin;
    gpio_init(sensor->pin);
}

bool dht11_read(dht11_t *sensor) {
    uint8_t data[5] = {0, 0, 0, 0, 0};
    uint8_t last_state = 1;
    uint8_t count = 0;
    uint8_t j = 0;

    // Starts signal
    gpio_set_dir(sensor->pin, GPIO_OUT);
    gpio_put(sensor->pin, 0);
    sleep_ms(18);
    gpio_put(sensor->pin, 1);
    sleep_us(30);
    
    gpio_set_dir(sensor->pin, GPIO_IN);

    // 3. BIT-BANGING LOOP
    // We look for 85 transitions (80 data bits + 5 response pulses)
    for (uint i = 0; i < 85; i++) {
        count = 0;
        while (gpio_get(sensor->pin) == last_state) {
            count++;
            sleep_us(1);
            if (count == 255) return false; // Timeout
        }
        last_state = gpio_get(sensor->pin);

        if ((i >= 4) && (i % 2 == 0)) {
            data[j / 8] <<= 1;
            // DHT11 bit timing: ~26us is '0', ~70us is '1'
            if (count > 40) data[j / 8] |= 1; 
            j++;
        }
    }

    // Grab humidity and temperature from the array of data we have
    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
        sensor->humidity = data[0];
        sensor->temperature = data[2];
        return true;
    }

    return false;
}