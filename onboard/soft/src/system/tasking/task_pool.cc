#include "task_pool.h"

#include <radio>
#include <sensors>
#include <control>

namespace System {
namespace Tasking {
namespace Pool {

/* All task instances go here */
Radio::Digital::XBeeReadIdleTask xbeeReadIdleTask;
Radio::Digital::TelemetryTask telemetryTask;
Control::StabilizationAndEngineUpdateTask stabilizationAndEngineUpdateTask;
Control::MARG margTask;

/* All task instances end here */

Task* const tasks[asIntegral<unsigned int>(TaskType::TASK_TYPE_COUNT)] = {
    [TaskType::StabilizationAndEngineUpdateTask] = &stabilizationAndEngineUpdateTask,
    [TaskType::XBeeReadIdleTask] = &xbeeReadIdleTask,
    [TaskType::TelemetryTask] = &telemetryTask,
    [TaskType::MARGTask] = &margTask,
};

Task*       const getTask(TaskType type) { return tasks[asIntegral<unsigned int>(type)]; }

}
}
}