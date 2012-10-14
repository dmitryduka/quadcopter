#include "mpu6050.h"
#include <common/ct-utility.hpp>
#include <common/math/maths.hpp>
#include <system/bus/i2c/i2c.h>
#include <system/devices.hpp>
#include <system/util.h>
#include <system/registry.hpp>

namespace Sensors {
namespace IMU {
namespace MPU6050 {


static void mpu6050_setup(char reg, char byte) {
    System::Bus::I2C::start();
    System::Bus::I2C::write(MPU6050_ADDRESS_W);
    System::Bus::I2C::write(reg);
    System::Bus::I2C::write(byte);
    System::Bus::I2C::stop();
}

void init() {
    mpu6050_setup(MPU6050_PWR_MGMT, 0x80); //Device reset
    System::Util::delay(5_ms); //approx 5ms grace period
    mpu6050_setup(MPU6050_PWR_MGMT, MPU6050_PWR_MGMT_SLEEP_DISABLE);
    mpu6050_setup(MPU6050_SETUP_DLPF, MPU6050_DLPF_44HZ_DELAY_5MS);
    mpu6050_setup(MPU6050_SETUP_GYRO, MPU6050_GYRO_RANGE_2000DEG);
    mpu6050_setup(MPU6050_SETUP_ACC, MPU6050_ACC_RANGE_16G);
    System::Util::delay(1_ms);
}

/* Whole operation takes ~400us, so no need for separate I2C tasks */
void updateAccelerometerAndGyro() {
    System::Bus::I2C::start();
    System::Bus::I2C::write(MPU6050_ADDRESS_W);
    System::Bus::I2C::write(MPU6050_ACC_ADDR);
    System::Bus::I2C::stop();

    System::Bus::I2C::start();
    System::Bus::I2C::write(MPU6050_ADDRESS_R);

    int ax  = System::Bus::I2C::read();
    int axL = System::Bus::I2C::read();
    int ay  = System::Bus::I2C::read();
    int ayL = System::Bus::I2C::read();
    int az  = System::Bus::I2C::read();
    int azL = System::Bus::I2C::read();

    /* Skip TEMP_OUT_H & TEMP_OUT_L */
    System::Bus::I2C::read();
    System::Bus::I2C::read();

    int gx  = System::Bus::I2C::read();
    int gxL = System::Bus::I2C::read();
    int gy  = System::Bus::I2C::read();
    int gyL = System::Bus::I2C::read();
    int gz  = System::Bus::I2C::read();
    int gzL = System::Bus::I2C::read();
    System::Bus::I2C::stop();

    ax = Math::sign_extend((ax << 8) | axL);
    ay = Math::sign_extend((ay << 8) | ayL);
    az = Math::sign_extend((az << 8) | azL);
    gx = Math::sign_extend((gx << 8) | gxL);
    gy = Math::sign_extend((gy << 8) | gyL);
    gz = Math::sign_extend((gz << 8) | gzL);

    System::Registry::set(System::Registry::ACCELEROMETER1_X, ax);
    System::Registry::set(System::Registry::ACCELEROMETER1_Y, ay);
    System::Registry::set(System::Registry::ACCELEROMETER1_Z, az);
    System::Registry::set(System::Registry::GYRO_X, gx);
    System::Registry::set(System::Registry::GYRO_Y, gy);
    System::Registry::set(System::Registry::GYRO_Z, gz);
}

void updateTemperature() {
    /* not implemented */
}

void updatePressure() {
    /* not implemented */
}

}
}
}
