#include "stabilization.h"
#include "devices.hpp"
#include "registry.hpp"
#include "mpu6050.h"

StabilizationAndEngineUpdateTask::StabilizationAndEngineUpdateTask() : ix(0), iy(0), ox(0), oy(0) {}

/* Calculate all corrections and apply them to motors */
void StabilizationAndEngineUpdateTask::start() {
    /* Update IMU data */
    IMUUpdateTask::update();
    /* Calculate engine corrections */
    horizontalStabilization();
    /* Also azimuth control here */

    int throttle = SystemRegistry::value(SystemRegistry::THROTTLE);
    int E1, E2, E3, E4;
    E1 = E2 = E3 = E4 = throttle;
    if (throttle > MINIMUM_THROTTLE_FOR_CORRECTIONS) {
        E1 += SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
        E2 -= SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
        E3 -= SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
        E4 += SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
    }
    eng_ctrl(E1, E3, ENGINES_13_ADDR);
    eng_ctrl(E2, E4, ENGINES_24_ADDR);
}

/* PID-controller */
void StabilizationAndEngineUpdateTask::horizontalStabilization() {
    int ACC_X = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_X);
    int ACC_Y = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_Y);
    /* Actual sensors are tilted 45 deg. */
    int x = ACC_X - ACC_Y;
    int y = ACC_X + ACC_Y;

    /* P term */
    int px = x - SystemRegistry::value(SystemRegistry::DESIRED_X);
    int py = y - SystemRegistry::value(SystemRegistry::DESIRED_Y);

    /* D term */
    int gx = SystemRegistry::value(SystemRegistry::GYRO_X);
    int gy = SystemRegistry::value(SystemRegistry::GYRO_Y);

    int dx = - (gx + gy);
    int dy = gx - gy;

    /* I term */
    if ((x ^ ox) >> 31) ix = 0;
    else if (ix < I_MAX) ix += x;
    if ((y ^ oy) >> 31) iy = 0;
    else if (iy < I_MAX) iy += y;

    ox = x;
    oy = y;

    SystemRegistry::set(SystemRegistry::PID_CORRECTION_X, 	scale<Kp, 1024>(px) +
                        scale<Ki, 1024>(ix) +
                        scale<Kd, 1024>(dx));
    SystemRegistry::set(SystemRegistry::PID_CORRECTION_Y,	scale<Kp, 1024>(py) +
                        scale<Ki, 1024>(iy) +
                        scale<Kd, 1024>(dy));
}

