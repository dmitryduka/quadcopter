#include <common/ct-utility.hpp>
#include <system/tasking/task_pool.h>
#include <system/util.h>
#include <system/fp/float32.h>

namespace Tasks = System::Tasking::Pool;

int main() {
    float32 a(1.234f), b(1.0f), c(3.0f), d(3.14159f);
    System::Util::init();
    System::Tasking::TaskScheduler scheduler;

    volatile float32 e = a * b + c - d / a;
    scheduler.addTask(static_cast<System::Tasking::IdleTask*>(Tasks::getTask(Tasks::TaskType::XBeeReadIdleTask)));
    scheduler.addTask(static_cast<System::Tasking::ContinuousTask*>(Tasks::getTask(Tasks::TaskType::StabilizationAndEngineUpdateTask)), 440_hz);

    /* Forever */
    scheduler.start();
}

//=====================================================//
