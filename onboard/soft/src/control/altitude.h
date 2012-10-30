#ifndef ALTITUDE_H
#define ALTITUDE_H

#include <common>
#include <system>

namespace Control {
/* This task is a simple 2-state state machine: it either requests barometer to 
    start conversion (which takes considerable amount of time) or requests it to
    read pressure/temperature values */
class AltitudeTask : public System::Tasking::ContinuousTask {
private:
    enum State { GET_PRESSURE_AND_START_TEMPERATURE_CONVERSION, GET_TEMPERATURE_AND_START_PRESSURE_CONVERSION };
    State state;
    mean_filter<int, 32> filter;

    void calculateAltitude();
public:
    AltitudeTask();
    void start();
};

}

#endif