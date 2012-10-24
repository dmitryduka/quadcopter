#include "stabilization.h"

namespace Control {

StabilizationAndEngineUpdateTask::StabilizationAndEngineUpdateTask() : ix(0), iy(0), ox(0), oy(0) {}

/* Calculate all corrections and apply them to motors */
void StabilizationAndEngineUpdateTask::start() {
    /* Calculate engine corrections */
    horizontalStabilization();
    /* Also azimuth control here */

    unsigned int throttle = System::Registry::value(System::Registry::THROTTLE);
    int E1, E2, E3, E4;
    E1 = E2 = E3 = E4 = throttle;
    if (throttle > MINIMUM_THROTTLE_FOR_CORRECTIONS) {
        E1 += System::Registry::value(System::Registry::PID_CORRECTION_X);
        E2 -= System::Registry::value(System::Registry::PID_CORRECTION_Y);
        E3 -= System::Registry::value(System::Registry::PID_CORRECTION_X);
        E4 += System::Registry::value(System::Registry::PID_CORRECTION_Y);
    }
    *DEV_ENG_13 = ((E1 << 16) | E3);
    *DEV_ENG_24 = ((E2 << 16) | E4);
}

/* PID-controller */
void StabilizationAndEngineUpdateTask::horizontalStabilization() {
    /* TODO: Implement this */
}

}

