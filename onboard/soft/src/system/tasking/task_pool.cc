#include "task_pool.h"

#include <radio/xbee/xbee.h>
#include <sensors/imu/mpu6050.h>
#include <control/stabilization.h>
#include <control/marg.h>

namespace System {
namespace Tasking {
namespace Pool {

/* All task instances go here */
Radio::Digital::XBeeReadIdleTask xbeeReadIdleTask;
Control::StabilizationAndEngineUpdateTask stabilizationAndEngineUpdateTask;

/* All task instances end here */

Task* const tasks[asIntegral<unsigned int>(TaskType::TASK_TYPE_COUNT)] = {
    [TaskType::StabilizationAndEngineUpdateTask] = &stabilizationAndEngineUpdateTask,    
    [TaskType::XBeeReadIdleTask] = &xbeeReadIdleTask
};

Task*       const getTask(TaskType type) { return tasks[asIntegral<unsigned int>(type)]; }

}
}
}