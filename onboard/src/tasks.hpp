#ifndef TASKS_HPP
#define TASKS_HPP

#include "ct-utility.hpp"
#include "devices.hpp"
#include "registry.hpp"
#include "messages.hpp"

class TaskScheduler;
class Task;

/* Task base class definition.
    TaskScheduler needs each Task to hold its time to execute at,
    interval, nextTask (to construct a linked list structure, a queue).

    Do not inherit from this class - use either ContinuousTask, OneShotTask or IdleTask.
*/
class Task {
private:
    unsigned int		executeAt;
    unsigned int		interval;

    Task*		nextTask;
protected:
    TaskScheduler*	scheduler;

    void setScheduler(TaskScheduler* s) {
        scheduler = s;
    }
public:
    friend class TaskScheduler;

    Task() : executeAt(0), interval(0), nextTask(0) { }
    virtual void start() {}
};

/* Inherit from some of these classes and implement start() function as a task activity.
    Task can not be interrupted in the middle of execution, so do not implement blocking tasks!

    IdleTasks should not block at all to minimize scheduling errors.
*/
class IdleTask : public Task {};
class ContinuousTask : public Task {};
class OneShotTask : public Task {};

/* Task scheduler

    Create TaskScheduler object, add some tasks and call start()
    TaskScheduler class maintains three queues of tasks - one for continuous tasks, one for
    tasks which should be run only once and the last for the idle task, which will run everytime 
    the scheduler sees it's not the time yet to run some Continuous/OneShot tasks.

    Use
	* addTask(ContinuousTask*, int interval) - returns a pointer to the added task (later you might want to remove it). Scheduler owns the task though.
	* addTask(OneShotTask*, int interval) - =same=
	* addTask(IdleTask*) - =same=

	* removeTask(OneShotTask*, int interval) -  remove a task from queue (it'll try to find in both queues) and destroy the task
	* start() - starts infinite loop, requires RTC() function
	    - polls RTC until it's time to execute the next task. Runs idle tasks (in a loop) whenever it's possible. Each time some task was executed, it finds next task to be executed.
	    - it calls removeTask for tasks inherited from OneShotTask class
*/
class TaskScheduler {
private:
    Task*	idleTasks;
    Task*	continuousTasks;
    Task*	oneShotTasks;

    Task*	currentIdleTask;

    /* No need for IDLE_QUEUE here, because this is only for the logic to delete task from the oneShotTasks queue */
    enum Queues { CONTINUOUS_QUEUE= 0, ONE_SHOT_QUEUE, QUEUES_COUNT };

    /* holds a task to be executed */
    struct {
        Task*		task;
        unsigned int	time;
        int		queue;
    }		nextTask;

    /* Sets nextTask */
    void selectNextTask(unsigned int rtc) {
        /* find a task with minimum executeAt time */
        Task* nextTaskCandidate = 0;
        int queueNo = 0;
        unsigned int minimumExecuteAt = 0xFFFFFFFF;
        Task** queues[QUEUES_COUNT] = {&continuousTasks, &oneShotTasks};
        Task* queue = 0;
        for (int i = 0; i < QUEUES_COUNT; ++i) {
            Task* q = *(queues[i]);
            if (q) {
                Task* cur = q;
                /* This piece of code finds the minimum time between *now* (which is passed in the argument)
                and some task' time to execute.
                */
                while (cur) {
                    if (cur->executeAt - rtc < minimumExecuteAt) {
                        nextTaskCandidate = cur;
                        minimumExecuteAt = cur->executeAt - rtc;
                        queue = q;
                    }
                    cur = cur->nextTask;
                }
            }
        }
        return setNextTask(nextTaskCandidate, queue);
    }

    /* Sets up nextTask structure (which is used in the main loop of the scheduler) */
    void setNextTask(Task* t, Task* list) {
        nextTask.task = t;
        nextTask.time = t->executeAt;
        nextTask.queue = (list == continuousTasks) ? CONTINUOUS_QUEUE : ONE_SHOT_QUEUE;
    }

    /* Adds Task t to the queue ('list') */
    void addTask(Task** list, Task* t, int delay) {
        t->setScheduler(this);
        t->interval = delay;
        int rtc = RTC();
        t->executeAt = rtc + delay;
        Task* last = *list;
        /* First task */
        if (!last) {
            last = t;
            *list = t;
        } else {
            /* go to the end of the linked list.
            TODO: Scheduler should know about the last task in both linked lists */
            while (last->nextTask) last = last->nextTask;
            last->nextTask = t;
        }
    }
public:
    TaskScheduler() : idleTasks(0), continuousTasks(0), oneShotTasks(0), currentIdleTask(0), nextTask {0, 0, 0} {}
    ~TaskScheduler() { }

    /* The main loop - poll RTC, execute task if RTC > nextTask.time, execute next IdleTask otherwise */
    void start() {
        selectNextTask(0);
        forever {
            unsigned int rtc = RTC();
            /* Check if RTC value is equal or higher than nextTask' execution time,
            accounting that they could be on different sides of RTC overflow.
            MAX_TASK_INTERVAL_TICKS is defined in the devices.hpp and is 'once in a minute' */
            if (rtc >= nextTask.time && (rtc - nextTask.time) < MAX_TASK_INTERVAL_TICKS) {
                nextTask.task->start();
                if (nextTask.queue == ONE_SHOT_QUEUE) removeTask(nextTask.task);
                else nextTask.task->executeAt = rtc + nextTask.task->interval;
                selectNextTask(rtc);
            } else {
        	/* Select next idle task */
        	if(currentIdleTask) currentIdleTask = currentIdleTask->nextTask;
        	/* If it was the last one, jump to the start */
        	if(currentIdleTask == 0)
        	    currentIdleTask = idleTasks;
        	/* Execute selected task, if there is one */
        	if(currentIdleTask) currentIdleTask->start();
            }
        }
    }

    ContinuousTask*	addTask(ContinuousTask* t, int delay) {
        addTask(&continuousTasks, t, delay);
        return t;
    }

    IdleTask*		addTask(IdleTask* t) {
        addTask(&idleTasks, t, 0);
        return t;
    }

    OneShotTask*	addTask(OneShotTask* t, int delay) {
        addTask(&oneShotTasks, t, delay);
        return t;
    }

    /* Remove the task from queue and delete it (even if it is not in any queue) */
    void		removeTask(Task* t) {
        /* Look in both queues */
        int queueNo = -1;
        Task** queues[3] = {&idleTasks, &continuousTasks, &oneShotTasks};
        /* Find previous task */
        for (int i = 0; i < 3; ++i) {
            Task* queue = *(queues[i]);
            if (queue) {
                /* There is no previous task, because it is itself the first one */
                if (queue == t) {
                    *(queues[i]) = t->nextTask;
                    queueNo = i;
                    break;
                } else {
                    Task* prev = queue;
                    /* Go through the linked list */
                    while (prev->nextTask && (prev->nextTask != t)) prev = prev->nextTask;
                    /* Not found this task (so move to the next queue or end searching), or ... */
                    if (prev->nextTask != t) continue;
                    else {
                        /* ... Either it's in between other tasks ... */
                        if (t->nextTask) prev->nextTask = t->nextTask;
                        /* ... or it is the last one */
                        else prev->nextTask = 0;
                        queueNo = i;
                        break;
                    }
                }
            }
        }

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

/* Updates the SystemRegistry with new data from the IMU */
class IMUUpdateTask : public ContinuousTask {
public:
    /* Whole operation takes ~400us, so no need for separate I2C tasks */
    virtual void start() {
        update();
    }

    static void update() {
        i2c_start();
        i2c_io(0x1D0);  //MPU6050 ADDR
        i2c_io(0x13B);  //Accel
        i2c_stop();

        i2c_start();
        i2c_io(0x1D1);
        int ax  = 0xFF & i2c_io(0x0FF);
        int axL = 0xFF & i2c_io(0x0FF);
        int ay  = 0xFF & i2c_io(0x0FF);
        int ayL = 0xFF & i2c_io(0x0FF);
        int az  = 0xFF & i2c_io(0x0FF);
        int azL = 0xFF & i2c_io(0x0FF);
        i2c_io(0x0FF);
        i2c_io(0x0FF);

        int gx  = 0xFF & i2c_io(0x0FF);
        int gxL = 0xFF & i2c_io(0x0FF);
        int gy  = 0xFF & i2c_io(0x0FF);
        int gyL = 0xFF & i2c_io(0x0FF);
        int gz  = 0xFF & i2c_io(0x0FF);
        int gzL = 0xFF & i2c_io(0x1FF);
        i2c_stop();

        ax = sign_extend((ax << 8) | axL);
        ay = sign_extend((ay << 8) | ayL);
        az = sign_extend((az << 8) | azL);
        gx = sign_extend((gx << 8) | gxL);
        gy = sign_extend((gy << 8) | gyL);
        gz = sign_extend((gz << 8) | gzL);

        SystemRegistry::set(SystemRegistry::ACCELEROMETER1_X, ax);
        SystemRegistry::set(SystemRegistry::ACCELEROMETER1_Y, ay);
        SystemRegistry::set(SystemRegistry::ACCELEROMETER1_Z, az);
        SystemRegistry::set(SystemRegistry::GYRO_X, gx);
        SystemRegistry::set(SystemRegistry::GYRO_Y, gy);
        SystemRegistry::set(SystemRegistry::GYRO_Z, gz);
    }
};

/* Azimuth stabilization */
class AzimuthStabilizationTask : public ContinuousTask {
private:
    static const unsigned int Kz = 100;
public:
    virtual void start() {
        update();
    }

    static void update() {
        int gz = scale<Kz, 1024>(SystemRegistry::value(SystemRegistry::GYRO_Z));
        /* TOD: Check signes here */
        SystemRegistry::set(SystemRegistry::AZIMUTH_CORRECTION_X, gz);
        SystemRegistry::set(SystemRegistry::AZIMUTH_CORRECTION_Y, -gz);
    }
};

/* Updates the SystemRegistry with new data from the ADC (for throttle) */
class ThrottleADCTask : public ContinuousTask {
public:
    virtual void start() {
        update();
    }

    static void update() {
        int adc = adc_read(2) >> 2;
        SystemRegistry::set(SystemRegistry::THROTTLE, adc);
        leds(adc >> 2);
    }
};

/* Sets engine speed from SystemRegistry values (THROTTLE and PID_CORRECTION_*)
    There is no need to have all tasks above as separate tasks, so this task
    calls their static functions to update IMU data/throttle data. 

    It's handy to have these as separate tasks though.
    */
class StabilizationAndEngineUpdateTask : public ContinuousTask {
private:
    static constexpr unsigned int Kp = 160;
    static constexpr unsigned int Ki = 3;
    static constexpr unsigned int Kd = 300;
    static constexpr unsigned int I_MAX = 2000;

    static constexpr unsigned int MINIMUM_THROTTLE = 100;
    static constexpr unsigned int MINIMUM_THROTTLE_FOR_CORRECTIONS = 200;

    int ix, iy, ox, oy;
public:
    StabilizationAndEngineUpdateTask() : ix(0), iy(0), ox(0), oy(0) {}

    /* Calculate all corrections and apply them to motors */
    virtual void start() {
        /* Read throttle */
        ThrottleADCTask::update();
        /* Update IMU data */
        IMUUpdateTask::update();
        /* Calculate engine corrections */
        horizontalStabilization();
        /* Also azimuth control here */

        int throttle = SystemRegistry::value(SystemRegistry::THROTTLE);
        int E1, E2, E3, E4;
        E1 = E2 = E3 = E4 = throttle;
        if (throttle > MINIMUM_THROTTLE_FOR_CORRECTIONS) {
            /* check, that engine wont stop, because of PID */
            if (E1 + SystemRegistry::value(SystemRegistry::PID_CORRECTION_X) > MINIMUM_THROTTLE)
                E1 += SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
            else E1 = MINIMUM_THROTTLE;
            if (E2 - SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y) > MINIMUM_THROTTLE)
                E2 -= SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
            else E2 = MINIMUM_THROTTLE;
            if (E3 - SystemRegistry::value(SystemRegistry::PID_CORRECTION_X) > MINIMUM_THROTTLE)
                E3 -= SystemRegistry::value(SystemRegistry::PID_CORRECTION_X);
            else E3 = MINIMUM_THROTTLE;
            if (E4 + SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y) > MINIMUM_THROTTLE)
                E4 += SystemRegistry::value(SystemRegistry::PID_CORRECTION_Y);
            else E4 = MINIMUM_THROTTLE;

            /* check, that engine wont stop, because of azimuth correction */
            if (E1 + SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_X) > MINIMUM_THROTTLE)
                E1 += SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_X);
            if (E2 + SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_Y) > MINIMUM_THROTTLE)
                E2 += SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_Y);
            if (E3 + SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_X) > MINIMUM_THROTTLE)
                E3 += SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_X);
            if (E4 + SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_Y) > MINIMUM_THROTTLE)
                E4 += SystemRegistry::value(SystemRegistry::AZIMUTH_CORRECTION_Y);
        }
        eng_ctrl(E1, E3, ENGINES_13_ADDR);
        eng_ctrl(E2, E4, ENGINES_24_ADDR);
    }

    /* PID-controller */
    void horizontalStabilization() {
        int ACC_X = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_X);
        int ACC_Y = SystemRegistry::value(SystemRegistry::ACCELEROMETER1_Y);
        /* Actual sensors are tilted 45 deg. */
        int x = ACC_X - ACC_Y;
        int y = ACC_X + ACC_Y;

        /* P term */
        int px = x - SystemRegistry::value(SystemRegistry::DESIRED_X);
        int py = y - SystemRegistry::value(SystemRegistry::DESIRED_Y);

        /* D term */
        int gx = SystemRegistry::value(SystemRegistry::GYRO_X);
        int gy = SystemRegistry::value(SystemRegistry::GYRO_Y);

        int dx = - (gx + gy);
        int dy = gx - gy;

        /* I term */
        if ((x ^ ox) >> 31) ix = 0;
        else if (ix < I_MAX) ix += x;
        if ((y ^ oy) >> 31) iy = 0;
        else if (iy < I_MAX) iy += y;

        ox = x;
        oy = y;

        SystemRegistry::set(SystemRegistry::PID_CORRECTION_X, 	scale<Kp, 1024>(px) +
                            scale<Ki, 1024>(ix) +
                            scale<Kd, 1024>(dx));
        SystemRegistry::set(SystemRegistry::PID_CORRECTION_Y,	scale<Kp, 1024>(py) +
                            scale<Ki, 1024>(iy) +
                            scale<Kd, 1024>(dy));
    }
};

class XBeeReadIdleTask : public IdleTask {
private:
    char UART_BUFFER[Messages::MAX_MESSAGE_LENGTH];
    unsigned int bytesSoFar;
    Messages::EntryType handler;
public:
    XBeeReadIdleTask() : bytesSoFar(0), handler{0, 0} {}
    void start() {
	/* TODO: poll uart, exit if unavailable */
	/* TODO */
	/* read it otherwise */
	char b = uart_read();
	/* Determine message size/handler, because first byte is always message type */
	if(bytesSoFar == 0)
	    handler = Messages::handlers[b];
	/* Put the byte to the buffer */
	UART_BUFFER[bytesSoFar++] = b;
	/* check if we have full message yet and call handler if so */
	if(bytesSoFar == handler.size) {
	    handler.handler(UART_BUFFER);
	    bytesSoFar = 0;
	    handler = {0, 0};
	}
    }
};

#endif