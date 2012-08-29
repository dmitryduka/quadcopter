#ifndef MPU6050_H
#define MPU6050_H

#include "tasks.h"

/* Updates the SystemRegistry with new data from the IMU */
class IMUUpdateTask : public ContinuousTask {
public:
    virtual void start();
    static void update();
};

#endif
