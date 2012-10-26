#include "altimeter.h"
#include <system>

namespace Sensors {
namespace Baro {

unsigned short int _C[MS561101BA_PROM_REG_COUNT];

void init() {
    /* TODO: read calibration from the sensor PROM */
}

void startTemperatureConversion(unsigned char OSR) {
    /* TODO: request sensor to start conversion */
}

void startPressureConversion(unsigned char OSR) {
    /* TODO: request sensor to start conversion */
}

void updatePressure() {
    /* TODO: read values from the sensor and update registry */
    //System::Registry::set(System::Registry::PRESSURE, );
}

void updateTemperature() {
    /* TODO: read values from the sensor and update registry */
    //System::Registry::set(System::Registry::PRESSURE, );
}

}
}
