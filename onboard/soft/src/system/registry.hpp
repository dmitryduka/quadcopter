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
        ANGLE_PSI,
        ANGLE_THETA,
        ANGLE_PHI,
        ANGLE_PSI_TRIM,
        ANGLE_THETA_TRIM,
        ANGLE_PHI_TRIM,
        FLOAT_VALUES_END
    };
    enum IntegerValue {
        VALUES_BEGIN = -1,
        /* Accelerometer 1 */
        ACCELEROMETER1_X = 0,
        ACCELEROMETER1_Y,
        ACCELEROMETER1_Z,
        ACCELEROMETER1_TRIM_X,
        ACCELEROMETER1_TRIM_Y,
        ACCELEROMETER1_TRIM_Z,
        /* Accelerometer 2 */
        ACCELEROMETER2_X,
        ACCELEROMETER2_Y,
        ACCELEROMETER2_Z,
        /* Gyroscope */
        GYRO_X,
        GYRO_Y,
        GYRO_Z,
        GYRO_TRIM_X,
        GYRO_TRIM_Y,
        GYRO_TRIM_Z,
        /* Compass */
        COMPASS_X,
        COMPASS_Y,
        COMPASS_Z,
        COMPASS_TRIM_X,
        COMPASS_TRIM_Y,
        COMPASS_TRIM_Z,
        /* MPU6050 temperature */
        MPU6050_TEMPERATURE,
        /* Barometer */
        TEMPERATURE,
        TEMPERATURE_TRIM,
        PRESSURE,
        PRESSURE_TRIM,
        ALTITUDE,
        ALTITUDE_TRIM,
        DESIRED_ALTITUDE,
        /* Throttle */
        THROTTLE,
        /* Angles in millidegrees */
        ANGLE_PITCH,
        ANGLE_YAW,
        ANGLE_ROLL,
        /* Desired angles in millidegrees */
        DESIRED_PITCH,
        DESIRED_YAW,
        DESIRED_ROLL,
        /* PID values */
	PITCH_ROLL_PID_P,
	PITCH_ROLL_PID_I,
        PITCH_ROLL_PID_D,
        PITCH_ROLL_PID_IMAX,
        HEADING_PID_P,
        HEADING_PID_I,
        HEADING_PID_D,
        HEADING_PID_IMAX,
        ALTITUDE_PID_P,
        ALTITUDE_PID_I,
        ALTITUDE_PID_D,
        ALTITUDE_PID_IMAX,
        /* PID corrections */
        PID_CORRECTION_X,
        PID_CORRECTION_Y,
        HEADING_CORRECTION,
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