#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define ADXL345_ADDR 0x53 

// Registers
#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32

void adxl345_init() {
    uint8_t buf[2];
    buf[0] = REG_DATA_FORMAT; buf[1] = 0x0B;
    i2c_write_blocking(I2C_PORT, ADXL345_ADDR, buf, 2, false);
    buf[0] = REG_POWER_CTL; buf[1] = 0x08;
    i2c_write_blocking(I2C_PORT, ADXL345_ADDR, buf, 2, false);
}

int main() {
    stdio_init_all();
    
    // Initialize Wi-Fi chip for the LED
    if (cyw43_arch_init()) return -1;

    // Initialize I2C (GP4 = SDA, GP5 = SCL)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    adxl345_init();

    uint8_t data[6];
    int16_t x, y, z;
    uint8_t reg = REG_DATAX0;

    while (true) {
        // Read 6 bytes of data starting from REG_DATAX0
        i2c_write_blocking(I2C_PORT, ADXL345_ADDR, &reg, 1, true);
        i2c_read_blocking(I2C_PORT, ADXL345_ADDR, data, 6, false);

        // Combine bytes (ADXL345 uses Little Endian: Low byte first)
        x = (int16_t)(data[1] << 8 | data[0]);
        y = (int16_t)(data[3] << 8 | data[2]);
        z = (int16_t)(data[5] << 8 | data[4]);

        // Print values (Scale: 3.9mg/LSB in Full Res mode)
        printf("X: %.2fg | Y: %.2fg | Z: %.2fg\n", x*0.0039, y*0.0039, z*0.0039);

        // Blink LED to show activity
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(100);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(400);
    }
}
