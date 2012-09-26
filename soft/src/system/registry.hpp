#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "ct-utility.hpp"

namespace System {

class Registry {
public:
    enum Value {
        VALUES_BEGIN = -1,

        /* Accelerometer 1 */
        ACCELEROMETER1_X = 0,
        ACCELEROMETER1_Y,
        ACCELEROMETER1_Z,
        /* Accelerometer 2 */
        ACCELEROMETER2_X,
        ACCELEROMETER2_Y,
        ACCELEROMETER2_Z,
        /* Gyroscope */
        GYRO_X,
        GYRO_Y,
        GYRO_Z,
        /* Compass */
        COMPASS_X,
        COMPASS_Y,
        COMPASS_Z,
        /* Barometer */
        TEMPERATURE,
        ALTIMETER,
        /* Throttle */
        THROTTLE,
        /* PID corrections */
        PID_CORRECTION_X,
        PID_CORRECTION_Y,
        /* PID corrections */
        AZIMUTH_CORRECTION_X,
        AZIMUTH_CORRECTION_Y,
        /* Desired angle to stabilize */
        DESIRED_X,
        DESIRED_Y,
        /* etc */

        VALUES_END
    };
private:
    int values[VALUES_END];
private:
    Registry() {
        for (int i = 0; i < VALUES_END; ++i) values[i] = 0;
    }
public:
    static Registry& instance() {
	static System::Registry inst;
	return inst;
    };

    static int& value(Value v) {
        return instance().values[v];
    }
    static void set(Value v, int value) {
        instance().values[v] = value;
    }
};

}
#endif