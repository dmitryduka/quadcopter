#include "tasks.h"
#include "util.h"
#include "uart.h"
#include <common>

namespace System {
namespace Tasking {

static void debugInt(const char* str, unsigned int x) {
    System::Bus::UART::write_waiting(str);
    System::Bus::UART::write_waiting(b32tohex(x));
    System::Bus::UART::write_waiting('\n');
}

Task::Task() : executeAt(0), 
		lastExecuteAt(0), 
		interval(0), 
		trueInterval(0), 
		nextTask(0) { }

void Task::start() {}

TaskScheduler::TaskScheduler() : idleTasks(0), 
				continuousTasks(0), 
				oneShotTasks(0), 
				currentIdleTask(0), 
				nextTask {0, 0}, 
				timeToWait(0), 
				lastRtc(0),
				currentTaskExecuteAt(0) {}

TaskScheduler& TaskScheduler::instance() {
    static TaskScheduler inst;
    return inst;
}

void TaskScheduler::start() {
    unsigned int rtc = *DEV_RTC;
    initializeTasks(rtc);
    selectNextTask(rtc);
    lastRtc = rtc;
    forever {
	rtc = *DEV_RTC;
        /* Check if RTC value is equal or higher than nextTask' execution time,
        accounting that they could be on different sides of RTC overflow.
        MAX_TASK_INTERVAL_TICKS is defined in the devices.hpp and is 'once in a minute' */
        unsigned int timePassed = rtc - lastRtc;
        if (nextTask.task && (timePassed >= timeToWait)) {
    	    unsigned int overdue = timePassed - timeToWait;
    	    currentTaskExecuteAt = nextTask.task->executeAt;
            nextTask.task->start();
            nextTask.task->trueInterval = rtc - nextTask.task->lastExecuteAt;
            nextTask.task->lastExecuteAt = rtc;
            /* Either remove the task from the ONE_SHOT_QUEUE, or increment the execution time
            for the CONTINUOUS_QUEUE task */
            if (nextTask.queue == ONE_SHOT_QUEUE) removeTask(nextTask.task);
            else nextTask.task->executeAt = rtc + nextTask.task->interval - overdue;
            selectNextTask(currentTaskExecuteAt);
            lastRtc = currentTaskExecuteAt;
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

void TaskScheduler::initializeTasks(unsigned int rtc) {
    Task** queues[QUEUES_COUNT] = {&continuousTasks, &oneShotTasks};
    for (int i = 0; i < QUEUES_COUNT; ++i) {
        Task* q = *(queues[i]);
        if (q) {
            while (q) {
        	q->executeAt = rtc + q->interval;
                q = q->nextTask;
            }
        }
    }
#if DEBUG_INFO_UART == 1
    System::Bus::UART::write_waiting("Tasks initialized\n");
#endif
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
    Task** queues[QUEUES_COUNT] = {&continuousTasks, &oneShotTasks};
    timeToWait = 0xFFFFFFFF;
    for (int i = 0; i < QUEUES_COUNT; ++i) {
        Task* q = *(queues[i]);
        if (q) {
            Task* cur = q;
            /* This piece of code finds the minimum time between *now* (which is passed in the argument)
            and some task' time to execute.
            */
            while (cur) {
                if (cur->executeAt - rtc < timeToWait) setNextTask(cur, q, rtc);
                cur = cur->nextTask;
            }
        }
    }
}

/* Sets up nextTask structure (which is used in the main loop of the scheduler) */
void TaskScheduler::setNextTask(Task* t, Task* list, unsigned int rtc) {
    if(t) {
	nextTask.task = t;
	nextTask.queue = (list == continuousTasks) ? CONTINUOUS_QUEUE : ONE_SHOT_QUEUE;
	timeToWait = t->executeAt - rtc;
    }
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

    selectNextTask(currentTaskExecuteAt);
}

void TaskScheduler::ps() {
    Task** queues[QUEUES_COUNT] = {&continuousTasks, &oneShotTasks};
    for (int i = 0; i < QUEUES_COUNT; ++i) {
        Task* q = *(queues[i]);
        while (q) {
    	    System::Bus::UART::write_waiting(q->_());
    	    System::Bus::UART::write_waiting(" : ");
    	    System::Bus::UART::write_waiting(f32todec(float32(CPU_FREQUENCY_HZ) / float32(q->trueInterval)));
    	    System::Bus::UART::write_waiting("hz\n");
    	    q = q->nextTask;
	}
    }
}

void TaskScheduler::top() {
    System::Bus::UART::write_waiting(f32todec(float32(cpuLoaded) / float32(CPU_FREQUENCY_HZ)));
    System::Bus::UART::write_waiting("%\n");
}

void TaskScheduler::ttw() {
    System::Bus::UART::write_waiting(b32todec(timeToWait));
    System::Bus::UART::write_waiting('\n');
}


}
}