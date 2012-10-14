#include "telemetry.h"
#include <radio/messages/messages.hpp>
#include <system/bus/uart/uart.h>
#include <system/registry.hpp>

namespace Radio {
namespace Digital {


void TelemetryTask::start() {
    Messages::QuatData data;
    data.q1 = System::Registry::value(System::Registry::ORIENTATION_Q1);
    data.q2 = System::Registry::value(System::Registry::ORIENTATION_Q2);
    data.q3 = System::Registry::value(System::Registry::ORIENTATION_Q3);
    data.q4 = System::Registry::value(System::Registry::ORIENTATION_Q4);
    Messages::send(data);
}

}
}