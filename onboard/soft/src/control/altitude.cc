#include "altitude.h"
#include <system>
#include <sensors>

#define SR System::Registry

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

    /* compute new altitude */
    /* TODO: Compute altitude, try avoiding floating point */
    float32 press(SR::value(SR::PRESSURE)); /* altitude in cm */
    float32 temp(SR::value(SR::TEMPERATURE)); /* altitude in cm */
    const float32 sea_press(101325.0f);
    const float32 factor(0.19022256f);
    const float32 one(1.0f);
    const float32 zero_temp(27315.0f);
    const float32 factor2(0.0065f);
    /* altitude in meter */
    float32 alt = ((f32::pow((sea_press / press), factor) - one) * (temp + zero_temp)) / factor2;
    SR::set(SR::ALTITUDE, alt);
}

}