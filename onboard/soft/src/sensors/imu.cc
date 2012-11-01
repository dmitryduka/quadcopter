#include "imu.h"
#include <common>
#include <system/i2c.h>
#include <system/registry.hpp>

namespace Sensors {
namespace IMU {
namespace MPU6050 {

namespace I2C = System::Bus::I2C;

static void mpu6050_setup(char reg, char byte) {
    I2C::start();
    I2C::write(MPU6050_ADDRESS_W);
    I2C::write(reg);
    I2C::write(byte);
    I2C::stop();
}

void init() {
    mpu6050_setup(MPU6050_PWR_MGMT, MPU6050_PWR_MGMT_RESET);
    System::delay(5_ms); //approx 5ms grace period
    mpu6050_setup(MPU6050_PWR_MGMT, MPU6050_PWR_MGMT_SLEEP_DISABLE);
    mpu6050_setup(MPU6050_SETUP_DLPF, MPU6050_DLPF_44HZ_DELAY_5MS);
    mpu6050_setup(MPU6050_SETUP_GYRO, MPU6050_GYRO_RANGE_2000DEG);
    mpu6050_setup(MPU6050_SETUP_ACC, MPU6050_ACC_RANGE_16G);
    System::delay(1_ms);

    mpu6050_setup(MPU6050_I2C_BYPASS, MPU6050_I2C_BYPASS_ENABLE);
    mpu6050_setup(MPU6050_I2C_MASTER, MPU6050_I2C_MASTER_DISABLE);
    System::delay(1_ms);
}

/* Whole operation takes ~400us, so no need for separate I2C tasks */
void update() {
    I2C::start();
    I2C::write(MPU6050_ADDRESS_W);
    I2C::write(MPU6050_ACC_ADDR);
    I2C::stop();

    I2C::start();
    I2C::write(MPU6050_ADDRESS_R);

    int ax  = I2C::read();
    int axL = I2C::read();
    int ay  = I2C::read();
    int ayL = I2C::read();
    int az  = I2C::read();
    int azL = I2C::read();

    int t = I2C::read();
    int tL = I2C::read();

    int gx  = I2C::read();
    int gxL = I2C::read();
    int gy  = I2C::read();
    int gyL = I2C::read();
    int gz  = I2C::read();
    int gzL = I2C::write((char)0xFF); /* End of transaction */

    I2C::stop();

    ax = Math::sign_extend((ax << 8) | axL);
    ay = Math::sign_extend((ay << 8) | ayL);
    az = Math::sign_extend((az << 8) | azL);
    gx = Math::sign_extend((gx << 8) | gxL);
    gy = Math::sign_extend((gy << 8) | gyL);
    gz = Math::sign_extend((gz << 8) | gzL);

    t = Math::sign_extend((t << 8) | tL);
    bool neg = t < 0;
    if(neg) t = -t;
    t = Math::divide(t * 100, 340);
    if(neg) t = -t;
    t += 3653;

    System::Registry::set(System::Registry::MPU6050_TEMPERATURE, t);
    System::Registry::set(System::Registry::ACCELEROMETER1_X, ax);
    System::Registry::set(System::Registry::ACCELEROMETER1_Y, ay);
    System::Registry::set(System::Registry::ACCELEROMETER1_Z, az);
    System::Registry::set(System::Registry::GYRO_X, gx);
    System::Registry::set(System::Registry::GYRO_Y, gy);
    System::Registry::set(System::Registry::GYRO_Z, gz);
}

}
}
}
