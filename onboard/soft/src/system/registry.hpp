#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <common>
#include <system>

namespace System {

class Registry {
public:
    enum FloatValue {
        FLOAT_VALUES_BEGIN = -1,
        /* Orientation quaternion */
        ORIENTATION_Q1 = 0,
        ORIENTATION_Q2,
        ORIENTATION_Q3,
        ORIENTATION_Q4,
        FLOAT_VALUES_END
    };
    enum IntegerValue {
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
        /* PID values */
        PID_P,
        PID_I,
        PID_D,
        /* PID corrections */
        PID_CORRECTION_X,
        PID_CORRECTION_Y,
        /* PID corrections */
        AZIMUTH_CORRECTION_X,
        AZIMUTH_CORRECTION_Y,
        /* Desired angles */
        DESIRED_PITCH,
        DESIRED_YAW,
        DESIRED_ROLL,
        /* etc */
        VALUES_END
    };
private:
    float32 fvalues[FLOAT_VALUES_END];
    int ivalues[VALUES_END];
private:
    Registry() {
        for (int i = 0; i < VALUES_END; ++i) ivalues[i] = 0;
        float32 zero(0.0f);
        for (int i = 0; i < FLOAT_VALUES_END; ++i) fvalues[i] = zero;
    }
public:
    static Registry& instance() {
	static System::Registry inst;
	return inst;
    };

    static int& value(IntegerValue v) {
        return instance().ivalues[v];
    }

    static float32& value(FloatValue v) {
        return instance().fvalues[v];
    }

    static void set(IntegerValue v, int value) {
        instance().ivalues[v] = value;
    }

    static void set(FloatValue v, const float32& value) {
        instance().fvalues[v] = value;
    }
};

}
#endif