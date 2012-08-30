#ifndef MPU6050_H
#define MPU6050_H

namespace Sensors {

namespace IMU {

namespace MPU6050 {
    constexpr unsigned char MPU6050_ADDRESS_W 	= 0xD0;
    constexpr unsigned char MPU6050_ADDRESS_R 	= 0xD1;
    constexpr unsigned char MPU6050_ACC_ADDR 	= 0x3B;
    constexpr unsigned char MPU6050_GYRO_ADDR 	= 0x43;

    /* Updates the SystemRegistry with new data from the IMU */
    void updateAccelerometerAndGyro();
    void updateTemperature();
    void updatePressure();
}

}

}

#endif
