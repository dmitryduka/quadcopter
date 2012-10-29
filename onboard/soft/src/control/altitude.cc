#include "altitude.h"
#include <common>
#include <system>
#include <sensors>

#define SR System::Registry

namespace Control {

/* Hypsometric formula is used to convert current pressure to altitude.
   (P0/P)^(1 / 5.2558797). It takes a lot of cycles to compule pow(x,y)
   for floating point numbers, so here is special function which computes
   specifically x^(1/5.2558797) using 4-order polynomial approximation with
   error < 1.8e-5 in the region of [-600, 5500] meters */
static float32 mypow(const float32& x) {
    const float32 P[5] = { float32(-0.0105f), float32(0.0780f), float32(-0.2461f), float32(0.4903f), float32(0.6883f)};
    const float32 xx = x * x;
    return P[0] * xx * xx + P[1] * xx * x + P[2] * xx + P[3] * x + P[4];
}

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
    float32 PF(SR::value(SR::PRESSURE)); /* altitude in cm */
    float32 temp(SR::value(SR::TEMPERATURE)); /* altitude in cm */
    const float32 PS(101325.0f);

    const float32 temp1 = mypow(PS / PF);
    const float32 alt = (temp1 - float32(1.0f)) * (temp + float32(27315.0f)) / float32(0.0065f);
    /* altitude in meters */
    SR::set(SR::ALTITUDE, filter((int)alt));
}

}