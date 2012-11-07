#include "stabilization.h"

namespace Control {

StabilizationAndEngineUpdateTask::StabilizationAndEngineUpdateTask() : 
					rollPID(SR::PITCH_ROLL_PID_P,
						SR::PITCH_ROLL_PID_I,
						SR::PITCH_ROLL_PID_D,
						SR::PITCH_ROLL_PID_IMAX,
						SR::ANGLE_ROLL,
						SR::DESIRED_ROLL),
					pitchPID(SR::PITCH_ROLL_PID_P,
						SR::PITCH_ROLL_PID_I,
						SR::PITCH_ROLL_PID_D,
						SR::PITCH_ROLL_PID_IMAX,
						SR::ANGLE_PITCH,
						SR::DESIRED_PITCH),
					headingPID(SR::HEADING_PID_P,
						SR::HEADING_PID_I,
						SR::HEADING_PID_D,
						SR::HEADING_PID_IMAX,
						SR::ANGLE_YAW,
						SR::DESIRED_YAW),
					altitudePID(SR::ALTITUDE_PID_P,
						SR::ALTITUDE_PID_I,
						SR::ALTITUDE_PID_D,
						SR::ALTITUDE_PID_IMAX,
						SR::ALTITUDE,
						SR::DESIRED_ALTITUDE)
{
}

/* Calculate all corrections and apply them to motors */
void StabilizationAndEngineUpdateTask::start() {
    /* Calculate engine corrections */
    horizontalStabilization();
    /* Also azimuth control here */

    unsigned int throttle = System::Registry::value(System::Registry::THROTTLE);
    int E1, E2, E3, E4;
    E1 = E2 = E3 = E4 = throttle;
    if (throttle > MINIMUM_THROTTLE_FOR_CORRECTIONS) {
        E1 += SR::value(SR::PID_CORRECTION_X);
        E2 -= SR::value(SR::PID_CORRECTION_Y);
        E3 -= SR::value(SR::PID_CORRECTION_X);
        E4 += SR::value(SR::PID_CORRECTION_Y);
    }
    *DEV_ENG_13 = ((E1 << 16) | E3);
    *DEV_ENG_24 = ((E2 << 16) | E4);
}

void StabilizationAndEngineUpdateTask::horizontalStabilization() {
    SR::set(SR::PID_CORRECTION_X, rollPID());
    SR::set(SR::PID_CORRECTION_Y, pitchPID());
}

void StabilizationAndEngineUpdateTask::headingStabilization() {
    SR::set(SR::HEADING_CORRECTION, headingPID());
}

void StabilizationAndEngineUpdateTask::altitudeStabilization() {
    SR::set(SR::THROTTLE, altitudePID());
}

}

