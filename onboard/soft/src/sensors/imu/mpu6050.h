#ifndef MPU6050_H
#define MPU6050_H

namespace Sensors {

namespace IMU {

namespace MPU6050 {
    constexpr unsigned char MPU6050_ADDRESS_W 	= 0xD0;
    constexpr unsigned char MPU6050_ADDRESS_R 	= 0xD1;
    constexpr unsigned char MPU6050_ACC_ADDR 	= 0x3B;
    constexpr unsigned char MPU6050_GYRO_ADDR 	= 0x43;

    /* DLPF setup register */
    constexpr unsigned char MPU6050_SETUP_DLPF 	= 0x1A;
    constexpr unsigned char MPU6050_DLPF_260HZ_DELAY_0MS	= 0x0;
    constexpr unsigned char MPU6050_DLPF_184HZ_DELAY_2MS	= 0x1;
    constexpr unsigned char MPU6050_DLPF_94HZ_DELAY_3MS		= 0x2;
    constexpr unsigned char MPU6050_DLPF_44HZ_DELAY_5MS		= 0x3;
    constexpr unsigned char MPU6050_DLPF_21HZ_DELAY_9MS		= 0x4;
    constexpr unsigned char MPU6050_DLPF_10HZ_DELAY_14MS	= 0x5;
    constexpr unsigned char MPU6050_DLPF_5HZ_DELAY_19MS		= 0x6;

    /* Gyro setup register */
    constexpr unsigned char MPU6050_SETUP_GYRO 	= 0x1B;
    constexpr unsigned char MPU6050_GYRO_RANGE_250DEG 	= 0x0;
    constexpr unsigned char MPU6050_GYRO_RANGE_500DEG 	= 0x8;
    constexpr unsigned char MPU6050_GYRO_RANGE_1000DEG 	= 0x10;
    constexpr unsigned char MPU6050_GYRO_RANGE_2000DEG 	= 0x18;

    /* Accelerometer setup register */
    constexpr unsigned char MPU6050_SETUP_ACC 	= 0x1C;
    constexpr unsigned char MPU6050_ACC_RANGE_2G 	= 0x0;
    constexpr unsigned char MPU6050_ACC_RANGE_4G 	= 0x8;
    constexpr unsigned char MPU6050_ACC_RANGE_8G 	= 0x10;
    constexpr unsigned char MPU6050_ACC_RANGE_16G 	= 0x18;

    /* Power management setup register */
    constexpr unsigned char MPU6050_PWR_MGMT	= 0x6B;
    constexpr unsigned char MPU6050_PWR_MGMT_SLEEP_DISABLE	= 0x0;

    void init();
    /* Updates the SystemRegistry with new data from the IMU */
    void updateAccelerometerAndGyro();
    void updateTemperature();
    void updatePressure();
}

}

}

#endif
