#include "task_pool.h"

#include <radio/xbee/xbee.h>
#include <sensors/imu/mpu6050.h>
#include <control/stabilization.h>

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

IdleTask*       const getIdleTask(TaskType type) { return static_cast<IdleTask* const>(tasks[asIntegral<unsigned int>(type)]); }
ContinuousTask* const getContinuousTask(TaskType type) { return static_cast<ContinuousTask* const>(tasks[asIntegral<unsigned int>(type)]); }
OneShotTask*    const getOneShotTask(TaskType type) { return static_cast<OneShotTask* const>(tasks[asIntegral<unsigned int>(type)]); }

}
}
}