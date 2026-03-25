#include "imu.h"
#include "hardware/uart.h" // Assume we have a UART library for communication
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <stdio.h>

//UART configuration
//UART (Universal Asynchronous Receiver/Transmitter) 
/**is a common hardware communication protocol used to interface with devices like IMUs. 
 * It allows for serial communication using a simple two-wire interface (TX and RX).
 * In this code, we will set up UART communication to read data from the IMU, which typically sends packets containing sensor readings.
 * The configuration includes setting the baud rate, defining the TX and RX pins, and implementing functions to read and parse the incoming data packets.
 * **/
//placeholder values - replace with actual pins and baud

#define imu_uart uart0
#define imu_baud 9600
#define imu_tx_pin 0 //IMU RX pin (connected to IMU TX)
#define imu_rx_pin 1 //IMU TX pin (connected to IMU RX)

//packet constants
#define packet_start 0x55
#define packet_length 11 //1 byte header + 10 bytes data

#define packet_accel 0x51
#define packet_gyro 0x52
#define packet_angle 0x53
#define packet_mag 0x54

//conversion scales
//ZFIX: divided by 32768.0f to correctly convert raw int16 to physical units
#define accel_scale (16.0f   / 32768.0f) //assuming ±16g range
#define gyro_scale  (2000.0f / 32768.0f) //assuming ±2000°/s range
#define angle_scale (180.0f  / 32768.0f) //assuming ±180° range

//internal state
static IMUData sensor_data = {0};

//mock data - verify math is correct without hardware
#if IMU_MOCK_HARDWARE

static uint8_t mock_packet[][packet_length] = {
    //accel: 0g x, 0g y, 1g z
    // FIX: all mock packets padded to 11 bytes (were 9 before)
    {0x55, 0x51, 0x00,0x00, 0x00,0x00, 0x00,0x08, 0x00,0x00, 0xAE},
    //gyro: 0°/s x, 0°/s y, 0°/s z
    {0x55, 0x52, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0xA7},
    //angle: 10 roll, 5 pitch, 45 yaw
    {0x55, 0x53, 0x1C,0x07, 0x8E,0x03, 0x00,0x20, 0x00,0x00, 0x5B},
};

static int mock_index = 0;
static int mock_byte_index = 0;


//mimics reading from UART by cycling through predefined packets, always says yes
static bool imu_uart_read(void){
    return true; //always have data
}

//mimics uart_getchar by returning bytes from the mock packets in sequence
static uint8_t imu_uart_getchar(void){
    uint8_t byte = mock_packet[mock_index][mock_byte_index];
    mock_byte_index++;
    if(mock_byte_index >= packet_length){
        mock_byte_index = 0;
        mock_index = (mock_index + 1) % 3; //cycle through accel, gyro, angle packets
    }
    return byte;
}

#else

//real UART reading functions
static bool imu_uart_read(void){
    return uart_is_readable(imu_uart);
}

static uint8_t imu_uart_getchar(void){
    return uart_getc(imu_uart); //FIX: was uart_getchar, correct function is uart_getc
}

#endif

//checksum function
static bool verify_checksum(const uint8_t* packet){
    uint8_t sum = 0;
    for(int i=0; i < packet_length - 1; i++){
        sum += packet[i];
    }
    return (sum == packet[10]);
}

//packet parser function
static void parse_packet(const uint8_t * packet){
    if(!verify_checksum(packet)) {
        printf("[IMU] Bad checksum, dropping packet\n");
        return; //invalid packet
    }
    
    //each packet has 4 signed 16-bit values starting at byte 2,
    //we only care about the first 3 for accel/gyro/angle, and mag
    int16_t v[4];
    for (int i=0; i < 4; i++){
        v[i] = (int16_t)(packet[2 + i*2] | (packet[3 + i*2] << 8));
    }

    switch (packet[1]){
        case packet_accel:
            sensor_data.accel_x = v[0] * accel_scale;
            sensor_data.accel_y = v[1] * accel_scale;
            sensor_data.accel_z = v[2] * accel_scale;
            break;
        
        case packet_gyro:
            sensor_data.gyro_x = v[0] * gyro_scale;
            sensor_data.gyro_y = v[1] * gyro_scale;
            sensor_data.gyro_z = v[2] * gyro_scale;
            break;
        
        case packet_angle:
            sensor_data.roll  = v[0] * angle_scale;
            sensor_data.pitch = v[1] * angle_scale;
            sensor_data.yaw   = v[2] * angle_scale;
            break;
        
        case packet_mag:
            sensor_data.mag_x = v[0];
            sensor_data.mag_y = v[1];
            sensor_data.mag_z = v[2];
            break;
    }
}

//sets up UART communication - call once at startup in main()
void imu_init(void){
#if IMU_MOCK_HARDWARE
    printf("[IMU] Mock mode - no hardware needed\n");
#else
    uart_init(imu_uart, imu_baud);
    gpio_set_function(imu_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(imu_rx_pin, GPIO_FUNC_UART);
    sleep_ms(200); //let the sensor boot up
    printf("[IMU] Hardware initialized on GPIO %d/%d\n", imu_tx_pin, imu_rx_pin);
#endif
}

//reads incoming bytes and builds complete packets - call every loop iteration
//returns true if a new packet was parsed
bool imu_update(void){
    static uint8_t buf[packet_length];
    static int idx = 0;
    bool got_packet = false;

    while(imu_uart_read()){
        uint8_t byte = imu_uart_getchar();

        if(idx == 0 && byte != packet_start) continue; //wait for header byte 0x55

        buf[idx++] = byte;

        if(idx == packet_length){
            parse_packet(buf);
            idx = 0; //reset for next packet
            got_packet = true;
        }
    }

    for (int i = 0; i < packet_length; i++){
        printf("%02X ", buf[i]);
    }

    return got_packet; //lets main.c know when fresh data arrived
}

//returns a copy of the latest sensor data
IMUData imu_get_data(void){
    return sensor_data;
}
