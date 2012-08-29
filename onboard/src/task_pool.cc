#include "task_pool.h"

#include "xbee.h"
#include "mpu6050.h"
#include "stabilization.h"

namespace TaskPool {
/* All task instances go here */
IMUUpdateTask imuUpdateTask;
XBeeReadIdleTask xbeeReadIdleTask;
StabilizationAndEngineUpdateTask stabilizationAndEngineUpdateTask;

/* All task instances end here */

Task* const tasks[asIntegral<unsigned int>(TaskType::TASK_TYPE_COUNT)] = {
    [TaskType::StabilizationAndEngineUpdateTask] = &stabilizationAndEngineUpdateTask,    
    [TaskType::IMUUpdateTask] = &imuUpdateTask,    
    [TaskType::XBeeReadIdleTask] = &xbeeReadIdleTask
};

IdleTask*       const getIdleTask(TaskType type) { return static_cast<IdleTask* const>(tasks[asIntegral<unsigned int>(type)]); }
ContinuousTask* const getContinuousTask(TaskType type) { return static_cast<ContinuousTask* const>(tasks[asIntegral<unsigned int>(type)]); }
OneShotTask*    const getOneShotTask(TaskType type) { return static_cast<OneShotTask* const>(tasks[asIntegral<unsigned int>(type)]); }

}
