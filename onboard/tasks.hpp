#ifndef TASKS_HPP
#define TASKS_HPP

#include "ct-utility.hpp"
#include "devices.hpp"
#include "registry.hpp"

class TaskScheduler;
class Task;

class Task {
private:
    long long	lastTimeExecuted;
    long long	executeAt;
    long long	interval;

    Task*	nextTask;
public:
    friend class TaskScheduler;

    Task() : lastTimeExecuted(0), executeAt(0), interval(0), nextTask(0) {}
    virtual void start() {}
};

class ContinuousTask : public Task {};
class OneShotTask : public Task {};

class TaskScheduler {
private:
    /* 1st queue */
    struct {
        Task*	tasks;
        int	count;
    } continuousTasks;

    /* 2st queue */
    struct {
        Task*	tasks;
        int	count;
    } oneShotTasks;

    enum Queues { CONTINUOUS_TASK = 0, ONE_SHOT_TASK, QUEUES_COUNT };

    /* holds a task to be executed */
    struct {
	Task*		task;
	long long	time;
	int		queue;
    }		nextTask;

    /* Sets nextTask */
    void selectNextTask() {
	/* find a task with minimum executeAt time */
	Task* nextTaskCandidate = 0; 
	int queueNo = 0;
	unsigned long long minimumExecuteAt = 0xFFFFFFFFFFFFFFFFul;
	Task** queues[QUEUES_COUNT] = {&continuousTasks.tasks, &oneShotTasks.tasks};
	for(int i = 0; i < QUEUES_COUNT; ++i) {
	    Task* queue = *(queues[i]);
	    if(queue) {
		Task* cur = queue;
		while(cur->nextTask)
		    if(cur->executeAt < minimumExecuteAt) {
			nextTaskCandidate = cur;
			minimumExecuteAt = cur->executeAt;
			queueNo = i;
		    }
	    }
	}
	nextTask.task = nextTaskCandidate;
	nextTask.time = minimumExecuteAt;
	nextTask.queue = queueNo;
    }

    /* Adds Task t to the list */
    void addTask(Task*& list, Task* t, int delay) {
        Task* last = list;
        /* First task */
	if(!last) {
	    list = t;
	    return;
	} else /* go to the end of the linked list */
	    while(last->nextTask) last = last->nextTask;
	
	last->nextTask = t;
    }
public:
    TaskScheduler() : continuousTasks{0, 0}, oneShotTasks{0, 0}, nextTask{0, 0, 0} {}
    ~TaskScheduler() { }

    void start() {
	forever {
	    if(RTC() >= nextTask.time) {
		nextTask.task->start();
		nextTask.task->executeAt += nextTask.task->interval;
		selectNextTask();
		/* Remove task if it is of OneShotTask type */
		if(nextTask.queue = ONE_SHOT_TASK) removeTask(nextTask.task);
	    }
	}
    }

    ContinuousTask*	addTask(ContinuousTask* t, int delay) { 
	addTask(continuousTasks.tasks, t, delay); 
	continuousTasks.count++; 
	return t; 
    }

    OneShotTask*	addTask(OneShotTask* t, int delay) { 
	addTask(oneShotTasks.tasks, t, delay);
	oneShotTasks.count++; 
	return t; 
    }

    /* Remove the task from queue and delete it (even if it is not in any queue) */
    void		removeTask(Task* t) {
	/* Look in both queues */
	Task** queues[2] = {&continuousTasks.tasks, &oneShotTasks.tasks};
	/* Find previous task */
	for(int i = 0; i < sizeof(queues) / sizeof(Task*); ++i) {
	    Task* queue = *(queues[i]);
	    if(queue) {
		/* There is no previous task, because it is itself the first one */
		if(queue == t) {
		    *(queues[i]) = t->nextTask;
		    break;
		} else {
		    Task* prev = queue;
		    /* Go through the linked list */
		    while(prev->nextTask && (prev->nextTask != t))
			prev = prev->nextTask;
		    /* Not found this task (so move to the next queue or end searching), or ... */
		    if(prev->nextTask != t) continue;
		    /* ... Either it's in between other tasks ... */
		    if(t->nextTask) prev->nextTask = t->nextTask;
		    /* ... or it is the last one */
		    else prev->nextTask = 0;
		}
	    }
	}

	delete t;
    }
};

class HorizontalStabilizationTask : public ContinuousTask {
private:
    static const unsigned int Kp = 180;
    static const unsigned int Kd = 2100;
    
    int xo, yo;
public:
    virtual void start() {
	int ACC_X = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_X);
	int ACC_Y = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_Y);
	/* Actual sensors are tilted 45 deg. */
	int x = ACC_X - ACC_Y;
	int y = ACC_X + ACC_Y;

	/* P term */
	int px = x - SystemRegistry::value(SystemRegistry::DESIRED_X);
	int py = y - SystemRegistry::value(SystemRegistry::DESIRED_Y);

	/* D term */
	int dx = x - xo;
	int dy = y - yo;
	/* Use this later */
	//int dx = SystemRegistry::value(SystemRegistry::GYRO_X);
	//int dy = SystemRegistry::value(SystemRegistry::GYRO_Y);

	xo = x; yo = y;

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