#ifndef IMU_H
#define IMU_H

#include <stdint.h>
#include <stdbool.h>

// Flip to 1 to test without hardware, 0 for real hardware
#define IMU_MOCK_HARDWARE 0

// FIX: renamed fields to match imu.c (accel_x/y/z, gyro_x/y/z)
// FIX: added mag_x/y/z to match packet_mag case in imu.c
typedef struct {
    float accel_x, accel_y, accel_z;
    float gyro_x,  gyro_y,  gyro_z;
    float roll, pitch, yaw;
    int16_t mag_x, mag_y, mag_z;
} IMUData;

/* Public API */

void imu_init(void);         // call once at startup
bool imu_update(void);       // call every loop, returns true when new data arrives
IMUData imu_get_data(void);  // get latest sensor readings

#endif
