#include "telemetry.h"
#include <system>
#include <sensors>
#include <radio>

namespace Radio {
namespace Digital {

#define SR System::Registry

void TelemetryTask::start() {
    /*Messages::QuatData data;
    data.q1 = System::Registry::value(System::Registry::ORIENTATION_Q1);
    data.q2 = System::Registry::value(System::Registry::ORIENTATION_Q2);
    data.q3 = System::Registry::value(System::Registry::ORIENTATION_Q3);
    data.q4 = System::Registry::value(System::Registry::ORIENTATION_Q4);
    Messages::send(data);*/
    /*
    System::Bus::UART::write_waiting(f32todec(System::Registry::value(System::Registry::ANGLE_PSI)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(f32todec(System::Registry::value(System::Registry::ANGLE_THETA)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(f32todec(System::Registry::value(System::Registry::ANGLE_PHI)));
    System::Bus::UART::write_waiting("\n");
                                           */
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::COMPASS_X)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::COMPASS_Y)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::COMPASS_Z)));
    System::Bus::UART::write_waiting("\n");

    /*
    System::Bus::UART::write_waiting("magn: ");
    System::Bus::UART::write_waiting(b32todec((unsigned int)SR::value(SR::ALTITUDE)));
    System::Bus::UART::write_waiting("\n");

    /*System::Bus::UART::write_waiting("press: ");
    System::Bus::UART::write_waiting(b32todec((unsigned int)SR::value(SR::PRESSURE)));
    System::Bus::UART::write_waiting("\n");*/

    /*System::Bus::UART::write_waiting("temp: ");
    System::Bus::UART::write_waiting(b32todec((unsigned int)SR::value(SR::TEMPERATURE)));
    System::Bus::UART::write_waiting("\n");*/

    /*
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::ACCELEROMETER1_X)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::ACCELEROMETER1_Y)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::ACCELEROMETER1_Z)));
    System::Bus::UART::write_waiting("\n");
    */

    /*Sensors::IMU::MPU6050::update();

    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::GYRO_X)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::GYRO_Y)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::GYRO_Z)));
    System::Bus::UART::write_waiting("\n");*/

}

}
}