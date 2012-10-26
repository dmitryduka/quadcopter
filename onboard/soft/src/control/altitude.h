#ifndef ALTITUDE_H
#define ALTITUDE_H

#include <system>

/* This task is a simple 2-state state machine: it either requests barometer to 
    start conversion (which takes considerable amount of time) or requests it to
    read pressure/temperature values */
class AltitudeTask : public System::Tasking::ContinuousTask {
private:
    enum State { START_CONVERSION, GET_PRESSURE };
    State state;

    void calculateAltitude();
public:
    AltitudeTask();
    void start();
};


#endif