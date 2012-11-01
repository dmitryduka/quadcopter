#ifndef MAGNETOMETER_TASK_H
#define MAGNETOMETER_TASK_H

#include <common>
#include <system>

namespace Control {
/* This task is a simple 2-state state machine: it either requests barometer to 
    start conversion (which takes considerable amount of time) or requests it to
    read pressure/temperature values */
class MagnetometerTask : public System::Tasking::ContinuousTask {
public:
    void start();
};

}

#endif