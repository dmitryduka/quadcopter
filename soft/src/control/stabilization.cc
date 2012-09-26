#include "stabilization.h"
#include <system/devices.hpp>
#include <system/registry.hpp>
#include <sensors/imu/mpu6050.h>

namespace Control {

StabilizationAndEngineUpdateTask::StabilizationAndEngineUpdateTask() : ix(0), iy(0), ox(0), oy(0) {}

/* Calculate all corrections and apply them to motors */
void StabilizationAndEngineUpdateTask::start() {
    Sensors::IMU::MPU6050::updateAccelerometerAndGyro();
    /* Calculate engine corrections */
    horizontalStabilization();
    /* Also azimuth control here */

    int throttle = System::Registry::value(System::Registry::THROTTLE);
    int E1, E2, E3, E4;
    E1 = E2 = E3 = E4 = throttle;
    if (throttle > MINIMUM_THROTTLE_FOR_CORRECTIONS) {
        E1 += System::Registry::value(System::Registry::PID_CORRECTION_X);
        E2 -= System::Registry::value(System::Registry::PID_CORRECTION_Y);
        E3 -= System::Registry::value(System::Registry::PID_CORRECTION_X);
        E4 += System::Registry::value(System::Registry::PID_CORRECTION_Y);
    }
    *ENGINES_13_ADDR = ((E1 << 16) | E3);
    *ENGINES_24_ADDR = ((E2 << 16) | E4);
}

/* PID-controller */
void StabilizationAndEngineUpdateTask::horizontalStabilization() {
    int ACC_X = System::Registry::value(System::Registry::ACCELEROMETER1_X);
    int ACC_Y = System::Registry::value(System::Registry::ACCELEROMETER1_Y);
    /* Actual sensors are tilted 45 deg. */
    int x = ACC_X - ACC_Y;
    int y = ACC_X + ACC_Y;

    /* P term */
    int px = x - System::Registry::value(System::Registry::DESIRED_X);
    int py = y - System::Registry::value(System::Registry::DESIRED_Y);

    /* D term */
    int gx = System::Registry::value(System::Registry::GYRO_X);
    int gy = System::Registry::value(System::Registry::GYRO_Y);

    int dx = - (gx + gy);
    int dy = gx - gy;

    /* I term */
    if ((x ^ ox) >> 31) ix = 0;
    else if (ix < I_MAX) ix += x;
    if ((y ^ oy) >> 31) iy = 0;
    else if (iy < I_MAX) iy += y;

    ox = x;
    oy = y;

    System::Registry::set(System::Registry::PID_CORRECTION_X, 	scale<Kp, 1024>(px) +
                        scale<Ki, 1024>(ix) +
                        scale<Kd, 1024>(dx));
    System::Registry::set(System::Registry::PID_CORRECTION_Y,	scale<Kp, 1024>(py) +
                        scale<Ki, 1024>(iy) +
                        scale<Kd, 1024>(dy));
}

}

