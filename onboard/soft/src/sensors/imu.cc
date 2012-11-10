#include "imu.h"
#include <common>
#include <system/i2c.h>
#include <system/registry.hpp>
#include <system/tasks.h>

namespace Sensors {
namespace IMU {
namespace MPU6050 {

namespace I2C = System::Bus::I2C;
#define SR System::Registry

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

    SR::set(SR::GYRO_TRIM_X, DEFAULT_GYRO_TRIM_X);
    SR::set(SR::GYRO_TRIM_Y, DEFAULT_GYRO_TRIM_Y);
    SR::set(SR::GYRO_TRIM_Z, DEFAULT_GYRO_TRIM_Z);
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

    SR::set(SR::MPU6050_TEMPERATURE, t);
    SR::set(SR::ACCELEROMETER1_X, ax);
    SR::set(SR::ACCELEROMETER1_Y, ay);
    SR::set(SR::ACCELEROMETER1_Z, az);
    SR::set(SR::GYRO_X, gx - SR::value(SR::GYRO_TRIM_X));
    SR::set(SR::GYRO_Y, gy - SR::value(SR::GYRO_TRIM_Y));
    SR::set(SR::GYRO_Z, gz - SR::value(SR::GYRO_TRIM_Z));
}

static int signed_divide_shift(int a, unsigned int b) {
    bool neg = false;
    if(a < 0) {
	neg = true;
	a = -a;
    }
    a >>= b;
    if(neg) a = -a;
    return a;
}

CalibrationTask::CalibrationTask() : firstRun(true) {}

void CalibrationTask::start() {
    if(firstRun) {
	acc_acc[0] = acc_acc[1] = acc_acc[2] = 
	acc_gyro[0] = acc_gyro[1] = acc_gyro[2] = cur_sample = 0;
	System::Bus::UART::write_waiting("Calibration started\n");
	firstRun = false;
    }

    update();

    acc_gyro[0] += SR::value(SR::GYRO_X);
    acc_gyro[1] += SR::value(SR::GYRO_Y);
    acc_gyro[2] += SR::value(SR::GYRO_Z);
    /*acc_acc[0] += SR::value(SR::ACCELEROMETER1_X);
    acc_acc[1] += SR::value(SR::ACCELEROMETER1_Y);
    acc_acc[2] += SR::value(SR::ACCELEROMETER1_Z);*/

    if(cur_sample >= SAMPLES_COUNT) {
	for(int i = 0; i < 3; ++i)
	    acc_gyro[i] = signed_divide_shift(acc_gyro[i], log_<SAMPLES_COUNT>::value);
	/*acc_acc[0] = signed_divide_shift(acc_acc[0], log_<SAMPLES_COUNT>::value);
	acc_acc[1] = signed_divide_shift(acc_acc[1], log_<SAMPLES_COUNT>::value);
	acc_acc[2] = signed_divide_shift(acc_acc[2], log_<SAMPLES_COUNT>::value);*/

	/* report calibration values and set them in the System Registry */
	SR::set(SR::GYRO_TRIM_X, acc_gyro[0]);
	SR::set(SR::GYRO_TRIM_Y, acc_gyro[1]);
	SR::set(SR::GYRO_TRIM_Z, acc_gyro[2]);
	/*SR::set(ACCELEROMETER1_TRIM_X, acc_acc[0]);
	SR::set(ACCELEROMETER1_TRIM_Y, acc_acc[1]);
	SR::set(ACCELEROMETER1_TRIM_Z, acc_acc[2]);*/
	System::Bus::UART::write_waiting("GX = ");
	System::Bus::UART::write_waiting(b32todec(acc_gyro[0]));
	System::Bus::UART::write_waiting('\n');
	System::Bus::UART::write_waiting("GY = ");
	System::Bus::UART::write_waiting(b32todec(acc_gyro[1]));
	System::Bus::UART::write_waiting('\n');
	System::Bus::UART::write_waiting("GZ = ");
	System::Bus::UART::write_waiting(b32todec(acc_gyro[2]));
	System::Bus::UART::write_waiting('\n');
	System::Bus::UART::write_waiting("Calibration stopped\n");
	firstRun = true;
	System::Tasking::TaskScheduler::instance().removeTask(this);
    } else cur_sample++;
}

}
}
}
