#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define ADXL345_ADDR 0x53 // Default I2C address (SDO to GND)

// ADXL345 Register Map
#define REG_DEVID       0x00
#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32

void adxl345_init() 