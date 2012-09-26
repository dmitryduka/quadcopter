#include <common/ct-utility.hpp>
#include <system/tasking/task_pool.h>
#include <system/util.h>

namespace Tasks = System::Tasking::Pool;

int main() {
    System::Util::init();
    System::Tasking::TaskScheduler scheduler;

    scheduler.addTask(static_cast<System::Tasking::IdleTask*>(Tasks::getTask(Tasks::TaskType::XBeeReadIdleTask)));
    scheduler.addTask(static_cast<System::Tasking::ContinuousTask*>(Tasks::getTask(Tasks::TaskType::StabilizationAndEngineUpdateTask)), 440_hz);

    /* Forever */
    scheduler.start();
}

//=====================================================//
