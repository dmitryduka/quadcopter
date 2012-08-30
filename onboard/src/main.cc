#include "common/ct-utility.hpp"
#include "system/tasking/task_pool.h"

namespace Tasks = System::Tasking::Pool;

int main() {
    /* TODO: init */
    System::Tasking::TaskScheduler scheduler;

    scheduler.addTask(Tasks::getIdleTask(Tasks::TaskType::XBeeReadIdleTask));
    scheduler.addTask(Tasks::getContinuousTask(Tasks::TaskType::StabilizationAndEngineUpdateTask), 440_hz);

    /* Forever */
    scheduler.start();
}

//=====================================================//
