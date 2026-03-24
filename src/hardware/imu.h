#ifndef IMU_H
#define IMU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
    float roll, pitch, yaw;
} imu_data_t;

/* Public API */

void imu_init(void);
bool imu_update(imu_data_t *data);

#endif
