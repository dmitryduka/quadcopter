#ifndef STABILIZATION_HPP
#define STABILIZATION_HPP

#include <system/tasking/tasks.h>

namespace Control {

/*  TODO: comments
    */
class StabilizationAndEngineUpdateTask : public System::Tasking::ContinuousTask {
private:
    static constexpr unsigned int Kp = 160;
    static constexpr unsigned int Ki = 3;
    static constexpr unsigned int Kd = 300;
    static constexpr unsigned int I_MAX = 2000;

    static constexpr unsigned int MINIMUM_THROTTLE = 100;
    static constexpr unsigned int MINIMUM_THROTTLE_FOR_CORRECTIONS = 200;

    int ix, iy, ox, oy;
public:
    StabilizationAndEngineUpdateTask();
    /* Calculate all corrections and apply them to motors */
    virtual void start();

    void horizontalStabilization();
};
}
#endif

