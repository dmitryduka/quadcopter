#include "tasks.h"
#include <common>

namespace System {
namespace Tasking {

Task::Task() : executeAt(0), interval(0), nextTask(0) { }
void Task::start() {}

TaskScheduler::TaskScheduler() : idleTasks(0), continuousTasks(0), oneShotTasks(0), currentIdleTask(0), nextTask {0, 0, 0} {}

void TaskScheduler::start() {
    selectNextTask(0);
    forever {
        unsigned int rtc = *DEV_RTC;
        /* Check if RTC value is equal or higher than nextTask' execution time,
        accounting that they could be on different sides of RTC overflow.
        MAX_TASK_INTERVAL_TICKS is defined in the devices.hpp and is 'once in a minute' */
        if (nextTask.task && (rtc - nextTask.time) < MAX_TASK_INTERVAL_TICKS) {
            nextTask.task->start();
            if (nextTask.queue == ONE_SHOT_QUEUE) removeTask(nextTask.task);
            else nextTask.task->executeAt = rtc + nextTask.task->interval;
            selectNextTask(rtc);
        } else if(idleTasks) { /* If there are any idle tasks */
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

ContinuousTask*	TaskScheduler::addTask(ContinuousTask* t, int delay) {
    addTask(&continuousTasks, t, delay);
    return t;
}

IdleTask*		TaskScheduler::addTask(IdleTask* t) {
    addTask(&idleTasks, t, 0);
    return t;
}

OneShotTask*	TaskScheduler::addTask(OneShotTask* t, int delay) {
    addTask(&oneShotTasks, t, delay);
    return t;
}

/* Remove the task from queue and delete it (even if it is not in any queue) */
void		TaskScheduler::removeTask(Task* t) {
    /* Look in both queues */
    Task** queues[3] = {&idleTasks, &continuousTasks, &oneShotTasks};
    /* Find previous task */
    for (int i = 0; i < 3; ++i) {
        Task* queue = *(queues[i]);
        if (queue) {
            /* There is no previous task, because it is itself the first one */
            if (queue == t) {
                *(queues[i]) = t->nextTask;
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
                    /* and exit */
                    break;
                }
            }
        }
    }

    t->scheduler = 0;
}

void TaskScheduler::selectNextTask(unsigned int rtc) {
    /* find a task with minimum executeAt time */
    Task* nextTaskCandidate = 0;
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
void TaskScheduler::setNextTask(Task* t, Task* list) {
    nextTask.task = t;
    nextTask.time = t->executeAt;
    nextTask.queue = (list == continuousTasks) ? CONTINUOUS_QUEUE : ONE_SHOT_QUEUE;
}

/* Adds Task t to the queue ('list') */
void TaskScheduler::addTask(Task** list, Task* t, int delay) {
    t->setScheduler(this);
    t->interval = delay;
    t->executeAt = *DEV_RTC + delay;
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

}
}