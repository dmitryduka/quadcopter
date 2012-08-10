#ifndef TASKS_HPP
#define TASKS_HPP

#include "ct-utility.hpp"
#include "devices.hpp"
#include "registry.hpp"

class TaskScheduler;
class Task;

/* Task base class definition.
    TaskScheduler needs each Task to hold its time to execute at,
    interval, nextTask (to construct a linked list structure, a queue).

    Do not inherit from this class - use either ContinuousTask or OneShotTask.
*/
class Task {
private:
    long long		executeAt;
    long long		interval;

    Task*		nextTask;
protected:
    TaskScheduler*	scheduler;

    void setScheduler(TaskScheduler* s) { scheduler = s; }
public:
    friend class TaskScheduler;

    Task() : executeAt(0), interval(0), nextTask(0) {}
    virtual void start() {}
};

/* Inherit from some of these classes and implement void start() function as a task activity. */
class ContinuousTask : public Task {};
class OneShotTask : public Task {};

/* Task scheduler

    Create TaskScheduler object, add some tasks and call start()
    TaskScheduler class maintains two queues of tasks - one for continuous tasks and one for
    tasks which should be run only once. 
    Use 
	* addTask(ContinuousTask*, int interval) - returns a pointer to the added task (later you might want to remove it). Scheduler owns the task though.
	* addTask(OneShotTask*, int interval)

	* removeTask(OneShotTask*, int interval) -  remove a task from queue (it'll try to find in both queues) and destroy the task
	* start() - starts infinite loop, requires RTC() function
	    - polls RTC until it's time to execute next task. Each time some task was executed, it finds next task to be executed.
	    - it calls removeTask for tasks inherited from OneShotTask class
*/
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

    enum Queues { CONTINUOUS_QUEUE= 0, ONE_SHOT_QUEUE, QUEUES_COUNT };

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
		while(cur) {
		    if(cur->executeAt < minimumExecuteAt) {
			nextTaskCandidate = cur;
			minimumExecuteAt = cur->executeAt;
			queueNo = i;
		    }
		    cur = cur->nextTask;
		}
	    }
	}
	nextTask.task = nextTaskCandidate;
	nextTask.time = minimumExecuteAt;
	nextTask.queue = queueNo;
    }

    /* Adds Task t to the list */
    void addTask(Task*& list, Task* t, int delay) {
	t->setScheduler(this);
	t->interval = delay;
	t->executeAt = RTC() + delay;
        Task* last = list;
        /* First task */
	if(!last) {
	    list = t;
	    last = list;
	    nextTask.task = last;
	    nextTask.time = delay;
	    if(list == continuousTasks.tasks) nextTask.queue = CONTINUOUS_QUEUE;
	    else nextTask.queue = ONE_SHOT_QUEUE;
	    return;
	} else /* go to the end of the linked list */
	    while(last->nextTask) last = last->nextTask;
	
	last->nextTask = t;
	/* update next task */
	selectNextTask();
    }
public:
    TaskScheduler() : continuousTasks{0, 0}, oneShotTasks{0, 0}, nextTask{0, 0, 0} {}
    ~TaskScheduler() { }

    void start() {
	forever {
	    if(RTC() > 72000000) break;
	    if(RTC() >= nextTask.time) {
		nextTask.task->start();
		nextTask.task->executeAt += nextTask.task->interval;
		selectNextTask();
		/* Remove task if it is of OneShotTask type */
		if(nextTask.queue == ONE_SHOT_QUEUE) removeTask(nextTask.task);
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
	int queueNo = -1;
	Task** queues[2] = {&continuousTasks.tasks, &oneShotTasks.tasks};
	/* Find previous task */
	for(int i = 0; i < QUEUES_COUNT; ++i) {
	    Task* queue = *(queues[i]);
	    if(queue) {
		/* There is no previous task, because it is itself the first one */
		if(queue == t) {
		    *(queues[i]) = t->nextTask;
		    queueNo = i;
		    break;
		} else {
		    Task* prev = queue;
		    /* Go through the linked list */
		    while(prev->nextTask && (prev->nextTask != t))
			prev = prev->nextTask;
		    /* Not found this task (so move to the next queue or end searching), or ... */
		    if(prev->nextTask != t) continue;
		    else {
			/* ... Either it's in between other tasks ... */
			if(t->nextTask) prev->nextTask = t->nextTask;
			/* ... or it is the last one */
			else prev->nextTask = 0;
			queueNo = i;
			break;
		    }
		}
	    }
	}

	if(queueNo == CONTINUOUS_QUEUE) continuousTasks.count--;
	else if(queueNo == ONE_SHOT_QUEUE) oneShotTasks.count--;

	/* Delete task anyway */
	delete t;
    }
};

/*	------------------------------ ALL TASKS ----------------------------
	---	All tasks communicate through the SystemRegistry	  ---
	---								  ---
	---								  ---
	---								  ---
	---								  ---
	---								  ---
	---								  ---
	------------------------------ ALL TASKS ---------------------------- */

/* This class implements PD-controller
*/
class HorizontalStabilizationTask : public ContinuousTask {
private:
    static const unsigned int Kp = 180;
    static const unsigned int Kd = 2100;

    int xo, yo;
public:
    HorizontalStabilizationTask() : xo(0), yo(0) {}
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
	/* Current implementation */
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

/* Updates the SystemRegistry with new data from the IMU */
class IMUUpdateTask : public ContinuousTask {
private:
    class I2C_Write_Task : public OneShotTask {
	private:
	    int byte;
	public:
	    I2C_Write_Task(int b) : byte(b) {}
	    virtual void start() {
		/* while(1) { ensure that the device is ready } */
		/* write the value */
	    }
    };

    class I2C_Read_Task : public OneShotTask {
	private:
	    int& value;
	public:
	    I2C_Read_Task(int& val) : value(val) {}
	    virtual void start() {
		/* while(1) { ensure that the device is ready } */
		/* read the value */
	    }
    };
public:
    virtual void start() {
	/* Some i2c code here */
	/* ... */
	/* Current implementation */
	/* Example code . Tasks timing goes here */
	scheduler->addTask(new I2C_Write_Task(0x01), (CPU_FREQUENCY_HZ / 50000) * 1);
	scheduler->addTask(new I2C_Write_Task(0x55), (CPU_FREQUENCY_HZ / 50000) * 2);
	scheduler->addTask(new I2C_Write_Task(0xAA), (CPU_FREQUENCY_HZ / 50000) * 3);
	scheduler->addTask(new I2C_Read_Task(SystemRegistry::value(SystemRegistry::ACCELEROMETER1_X)), (CPU_FREQUENCY_HZ / 50000) * 4);
    }
};

/* Updates the SystemRegistry with new data from the ADC (for throttle) */
class ThrottleADCTask : public ContinuousTask {
public:
    virtual void start() {
	SystemRegistry::set(SystemRegistry::THROTTLE, adc_read(2) >> 2);
    }
};

/* Sets engine speed from SystemRegistry values (THROTTLE and PID_CORRECTION_*) */
class EnginesUpdateTask : public ContinuousTask {
public:
    virtual void start() {
	int throttle = SystemRegistry::value(SystemRegistry::THROTTLE);
	int E1 = throttle + SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
	int E2 = throttle - SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
	int E3 = throttle - SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
	int E4 = throttle + SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
    }
};

#endif