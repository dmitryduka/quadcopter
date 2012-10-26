#include "altitude.h"
#include <sensors>

namespace Control {

AltitudeTask::AltitudeTask() : state(GET_PRESSURE_AND_START_TEMPERATURE_CONVERSION) {}

void AltitudeTask::start() {
    if(state == GET_PRESSURE_AND_START_TEMPERATURE_CONVERSION) {
	Sensors::Baro::updatePressure();
	Sensors::Baro::startTemperatureConversion(MS561101BA_OSR_4096);
	state = GET_TEMPERATURE_AND_START_PRESSURE_CONVERSION;
    } else if(state == GET_TEMPERATURE_AND_START_PRESSURE_CONVERSION) {
	Sensors::Baro::updateTemperature();
	Sensors::Baro::startPressureConversion(MS561101BA_OSR_4096);
	state = GET_PRESSURE_AND_START_TEMPERATURE_CONVERSION;
    }
}

}