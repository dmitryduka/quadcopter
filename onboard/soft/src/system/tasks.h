#ifndef TASKS_HPP
#define TASKS_HPP

#include "devices.hpp"

namespace System {
namespace Tasking {

class TaskScheduler;

/* Task base class definition.
    TaskScheduler needs each Task to hold its time to execute at,
    interval, nextTask (to construct a linked list structure, a queue).

    Do not inherit from this class - use either ContinuousTask, OneShotTask or IdleTask.
*/
class Task {
private:
    unsigned int		executeAt;
    unsigned int		interval;
    
    Task*			nextTask;
protected:
    TaskScheduler*	scheduler;

    inline void setScheduler(TaskScheduler* s) { scheduler = s; }
public:
    friend class TaskScheduler;

    Task();
    virtual void start();
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

    constexpr static unsigned int MAX_TASK_INTERVAL_TICKS = 60 * CPU_FREQUENCY_HZ;

    /* No need for IDLE_QUEUE here, because this is only for the logic to delete task from the oneShotTasks queue */
    enum Queues { CONTINUOUS_QUEUE= 0, ONE_SHOT_QUEUE, QUEUES_COUNT };

    /* holds a task to be executed */
    struct {
        Task*		task;
        int		queue;
    }		nextTask;

    unsigned int timeToWait;
    unsigned int lastRtc;

    /* Sets nextTask */
    void selectNextTask(unsigned int rtc);
    /* Sets up nextTask structure (which is used in the main loop of the scheduler) */
    void setNextTask(Task* t, Task* list, unsigned int rtc);
    /* Adds Task t to the queue ('list') */
    void addTask(Task** list, Task* t, int delay);
    /* Initializes all tasks - sets their executeAt time */
    void initializeTasks(unsigned int rtc);
public:
    TaskScheduler();

    /* The main loop - poll RTC, execute task if RTC > nextTask.time, execute next IdleTask otherwise */
    void start();

    ContinuousTask*	addTask(ContinuousTask* t, int delay);
    IdleTask*		addTask(IdleTask* t);
    OneShotTask*	addTask(OneShotTask* t, int delay);

    /* Remove the task from queue and delete it (even if it is not in any queue) */
    void		removeTask(Task* t);
};

#define ADD_IDLE_TASK(scheduler, task) scheduler.addTask(static_cast<System::Tasking::IdleTask*>(System::Tasking::Pool::getTask(System::Tasking::Pool::task)))
#define ADD_CONTINUOUS_TASK(scheduler, task, freq) scheduler.addTask(static_cast<System::Tasking::ContinuousTask*>(System::Tasking::Pool::getTask(System::Tasking::Pool::task)), freq)

}
}

#endif
