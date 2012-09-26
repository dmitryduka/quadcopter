#include <common/ct-utility.hpp>
#include <system/tasking/task_pool.h>
#include <system/util.h>
#include <system/fp/float32.h>

namespace Tasks = System::Tasking::Pool;

int main() {
    float32 a(1), b(1);
    System::Util::init();
    System::Tasking::TaskScheduler scheduler;

    volatile float32 c = a + b;
    scheduler.addTask(static_cast<System::Tasking::IdleTask*>(Tasks::getTask(Tasks::TaskType::XBeeReadIdleTask)));
    scheduler.addTask(static_cast<System::Tasking::ContinuousTask*>(Tasks::getTask(Tasks::TaskType::StabilizationAndEngineUpdateTask)), 440_hz);

    /* Forever */
    scheduler.start();
}

//=====================================================//
