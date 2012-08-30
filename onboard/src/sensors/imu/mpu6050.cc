#include "mpu6050.h"
#include <common/math/maths.hpp>
#include <system/bus/i2c/i2c.h>
#include <system/devices.hpp>
#include <system/registry.hpp>

namespace Sensors {
namespace IMU {
namespace MPU6050 {

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

    SystemRegistry::set(SystemRegistry::ACCELEROMETER1_X, ax);
    SystemRegistry::set(SystemRegistry::ACCELEROMETER1_Y, ay);
    SystemRegistry::set(SystemRegistry::ACCELEROMETER1_Z, az);
    SystemRegistry::set(SystemRegistry::GYRO_X, gx);
    SystemRegistry::set(SystemRegistry::GYRO_Y, gy);
    SystemRegistry::set(SystemRegistry::GYRO_Z, gz);
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

