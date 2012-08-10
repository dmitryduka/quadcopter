#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "ct-utility.hpp"
#include "memory_config.hpp"

class SystemRegistry;

SystemRegistry ** const SR = reinterpret_cast<SystemRegistry ** const>(SYSTEM_REGISTRY_INSTANCE_ADDRESS);

class SystemRegistry {
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
	ALTIMETER,
	/* Throttle */
	THROTTLE,
	/* PID corrections */
	PID_CORRECTION_X,
	PID_CORRECTION_Y,
	/* Desired angle to stabilize */
	DESIRED_X,
	DESIRED_Y,
	/* etc */

	VALUES_END
    };
private:
    int values[VALUES_END];
public:
    SystemRegistry() { 
	for(int i = 0; i < VALUES_END; ++i) values[0];
	*SR = this; 
    }

    static int& value(Value v) { return (*SR)->values[v]; }
    static void set(Value v, int value) { (*SR)->values[v] = value; }
};

#endif