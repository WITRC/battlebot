#ifndef IMU_H
#define IMU_H

#include <stdint.h>
#include <stdbool.h>


#define imu_baud    9600

#define packet_start   0x55
#define packet_length  11

#define packet_accel   0x51
#define packet_gyro    0x52
#define packet_angle   0x53
#define packet_mag     0x54

#define accel_scale (16.0f   / 32768.0f)
#define gyro_scale  (2000.0f / 32768.0f)
#define angle_scale (180.0f  / 32768.0f)



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
