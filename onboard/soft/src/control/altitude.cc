#include "altitude.h"
#include <sensors>

namespace Control {

AltitudeTask::AltitudeTask() : state(START_CONVERSION) {}

void AltitudeTask::start() {
    if(state == START_CONVERSION) {
	Sensors::Baro::startConversion(MS561101BA_OSR_4096);
    } else if(state == GET_PRESSURE) {
	Sensors::Baro::update();
	/* TODO: calculate altitude from pressure & temperature here */
	//System::Registry::set(System::Registry::ALTITUDE, altitude);
	state = START_CONVERSION;
    }
}

}