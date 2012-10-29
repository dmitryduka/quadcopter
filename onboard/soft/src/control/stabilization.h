#ifndef STABILIZATION_HPP
#define STABILIZATION_HPP

#include <system>
#include "pid.hpp"
namespace Control {

/*  TODO: comments
    */
class StabilizationAndEngineUpdateTask : public Tasks::ContinuousTask {
private:
    constexpr static unsigned int MINIMUM_THROTTLE_FOR_CORRECTIONS = 200;

    PID<External<SR::GYRO_X>> rollPID;
    PID<External<SR::GYRO_Y>> pitchPID;
    PID<Internal> headingPID;
    PID<Internal> altitudePID;
public:
    StabilizationAndEngineUpdateTask();
    virtual void start();

    void horizontalStabilization();
    void headingStabilization();
    void altitudeStabilization();
};
}
#endif

