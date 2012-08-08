#ifndef TASKS_HPP
#define TASKS_HPP

#include "ct-utility.hpp"
#include "devices.hpp"
#include "registry.hpp"

const unsigned int MAX_TASKS = 256;

class Task {
private:
    long long lastTimeExecuted;
    long long executeAt;
public:
    Task() : lastTimeExecuted(0), executeAt(0) {}
    virtual void start() {}
};

class ContinuousTask : public Task {};
class OneTimeTask : public Task {};

class TaskScheduler {
private:
    Task*	tasks[MAX_TASKS];
    int		tasksCount;
    struct {
	Task*		task;
	long int	time;
    }		nextTask;
public:
    TaskScheduler() : tasksCount(0) {}
    ~TaskScheduler() { }

    void start() {
	forever {
	    if(RTC() >= nextTask.time) {
		nextTask.task->start();
	    } else continue;
	}
    }

    void addTask(ContinuousTask*, int frequency) {    }
    void addTask(OneTimeTask*, int delay) {    }
};

class HorizontalStabilizationTask : public ContinuousTask {
private:
    static const unsigned int Kp = 180;
    static const unsigned int Kd = 2100;
public:
    virtual void start() {
	int ACC_X = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_X);
	int ACC_Y = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_Y);
	int x = ACC_X - ACC_Y;
	int y = ACC_X + ACC_Y;

	/* P term */
	int px = x - SystemRegistry::value(SystemRegistry::DESIRED_X);
	int py = y - SystemRegistry::value(SystemRegistry::DESIRED_Y);

	/* D term */
	int dx = SystemRegistry::value(SystemRegistry::GYRO_X);
	int dy = SystemRegistry::value(SystemRegistry::GYRO_Y);

	SystemRegistry::set(SystemRegistry::PID_CORRECTION_X, scale<Kp, 1024>(px) + scale<Kd, 1024>(dx));
	SystemRegistry::set(SystemRegistry::PID_CORRECTION_Y, scale<Kp, 1024>(py) + scale<Kd, 1024>(dy));
    }
};

class IMUUpdateTask : public ContinuousTask {
public:
    virtual void start() {
	/* Some i2c code here */
	/* ... */
	/* Current implementation */
	SystemRegistry::set(SystemRegistry::ACCELEROMETER1_X, *((int *)ACC_DATA_X));
	SystemRegistry::set(SystemRegistry::ACCELEROMETER1_Y, *((int *)ACC_DATA_Y));
    }
};

class ThrottleADCTask : public ContinuousTask {
public:
    virtual void start() {
	SystemRegistry::set(SystemRegistry::THROTTLE, adc_read(2) >> 2);
    }
};

class EnginesUpdateTask : public ContinuousTask {
public:
    virtual void start() {
	int throttle = SystemRegistry::value(SystemRegistry::THROTTLE);
	int E1 = throttle + SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
	int E2 = throttle - SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
	int E3 = throttle - SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
	int E4 = throttle + SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
	eng_ctrl(E1, E3, ENGINES_13_ADDR);
	eng_ctrl(E2, E4, ENGINES_24_ADDR);
    }
};

#endif