#include "telemetry.h"
#include <system>
#include <sensors>
#include <radio>

namespace Radio {
namespace Digital {


void TelemetryTask::start() {
    /*Messages::QuatData data;
    data.q1 = System::Registry::value(System::Registry::ORIENTATION_Q1);
    data.q2 = System::Registry::value(System::Registry::ORIENTATION_Q2);
    data.q3 = System::Registry::value(System::Registry::ORIENTATION_Q3);
    data.q4 = System::Registry::value(System::Registry::ORIENTATION_Q4);
    Messages::send(data);*/
    System::Bus::UART::write_waiting(b32todec((int)System::Registry::value(System::Registry::ANGLE_PSI)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec((int)System::Registry::value(System::Registry::ANGLE_THETA)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec((int)System::Registry::value(System::Registry::ANGLE_PHI)));
    System::Bus::UART::write_waiting("\n");

    /*
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::ACCELEROMETER1_X)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::ACCELEROMETER1_Y)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32todec(System::Registry::value(System::Registry::ACCELEROMETER1_Z)));
    System::Bus::UART::write_waiting("\n");
    */
    /*
    System::Bus::UART::write_waiting(b32tohex(System::Registry::value(System::Registry::GYRO_X)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32tohex(System::Registry::value(System::Registry::GYRO_Y)));
    System::Bus::UART::write_waiting(" ");
    System::Bus::UART::write_waiting(b32tohex(System::Registry::value(System::Registry::GYRO_Z)));
    System::Bus::UART::write_waiting("\n");
    */
}

}
}