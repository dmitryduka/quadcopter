#include "registry.hpp"
#include "mpu6050.h"

/* Whole operation takes ~400us, so no need for separate I2C tasks */
void IMUUpdateTask::start() {
    update();
}

void IMUUpdateTask::update() {
    i2c_start();
    i2c_io(0x1D0);  //MPU6050 ADDR
    i2c_io(0x13B);  //Accel
    i2c_stop();

    i2c_start();
    i2c_io(0x1D1);
    int ax  = 0xFF & i2c_io(0x0FF);
    int axL = 0xFF & i2c_io(0x0FF);
    int ay  = 0xFF & i2c_io(0x0FF);
    int ayL = 0xFF & i2c_io(0x0FF);
    int az  = 0xFF & i2c_io(0x0FF);
    int azL = 0xFF & i2c_io(0x0FF);
    i2c_io(0x0FF);
    i2c_io(0x0FF);

    int gx  = 0xFF & i2c_io(0x0FF);
    int gxL = 0xFF & i2c_io(0x0FF);
    int gy  = 0xFF & i2c_io(0x0FF);
    int gyL = 0xFF & i2c_io(0x0FF);
    int gz  = 0xFF & i2c_io(0x0FF);
    int gzL = 0xFF & i2c_io(0x1FF);
    i2c_stop();

    ax = sign_extend((ax << 8) | axL);
    ay = sign_extend((ay << 8) | ayL);
    az = sign_extend((az << 8) | azL);
    gx = sign_extend((gx << 8) | gxL);
    gy = sign_extend((gy << 8) | gyL);
    gz = sign_extend((gz << 8) | gzL);

    SystemRegistry::set(SystemRegistry::ACCELEROMETER1_X, ax);
    SystemRegistry::set(SystemRegistry::ACCELEROMETER1_Y, ay);
    SystemRegistry::set(SystemRegistry::ACCELEROMETER1_Z, az);
    SystemRegistry::set(SystemRegistry::GYRO_X, gx);
    SystemRegistry::set(SystemRegistry::GYRO_Y, gy);
    SystemRegistry::set(SystemRegistry::GYRO_Z, gz);
}

