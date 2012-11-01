#include "magnetometer.h"
#include <sensors>

namespace Control {

void MagnetometerTask::start() {
    Sensors::IMU::HMC5883::update();
}

}

