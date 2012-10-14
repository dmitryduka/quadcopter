#ifndef MARG_H
#define MARG_H

#include <system/tasking/tasks.h>
#include <system/fp/float32.h>

namespace Control {

class MARG : public System::Tasking::ContinuousTask {
private:
    // Global system variables
    unsigned int old_rtc;
    float32 deltat;
    float32 a_x, a_y, a_z; // accelerometer measurements
    float32 w_x, w_y, w_z; // gyroscope measurements in rad/s
    float32 m_x, m_y, m_z; // magnetometer measurements
    float32 SEq_1, SEq_2, SEq_3, SEq_4; // estimated orientation quaternion elements with initial conditions
    float32 b_x, b_z; // reference direction of flux in earth frame
    float32 w_bx, w_by, w_bz; // estimate gyroscope biases error

    void filterUpdateIMU(float32 w_x, float32 w_y, float32 w_z, float32 a_x, float32 a_y, float32 a_z);
    void filterUpdateMARG(float32 w_x, float32 w_y, float32 w_z, float32 a_x, float32 a_y, float32 a_z, float32 m_x, float32 m_y, float32 m_z);
public:
    MARG();

    void start();
};

}

#endif
