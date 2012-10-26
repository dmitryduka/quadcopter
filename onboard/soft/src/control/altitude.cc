#include "altitude.h"
#include <sensors>

AltitudeTask::AltitudeTask() : state(START_CONVERSION) {}

void AltitudeTask::calculateAltitude() {
    /* TODO: implement this */
}

void AltitudeTask::start() {
    if(state == START_CONVERSION) {
	Sensors::Baro::startConversion();
    } else if(state == GET_PRESSURE) {
	Sensors::Baro::updatePressure();
	calculateAltitude();
	System::Registry::setValue(System::Registry::ALTITUDE, altitude);
	state = START_CONVERSION;
    }
}