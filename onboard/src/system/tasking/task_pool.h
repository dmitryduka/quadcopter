#ifndef TASK_POOL_H
#define TASK_POOL_H
#include "tasks.h"

namespace System {
namespace Tasking {
namespace Pool {

enum class TaskType {
    TASK_TYPE_BEGIN = 0,
    StabilizationAndEngineUpdateTask = 0,
    XBeeReadIdleTask,
    TASK_TYPE_COUNT
};

IdleTask*       const     getIdleTask(TaskType type);
ContinuousTask* const     getContinuousTask(TaskType type);
OneShotTask*    const     getOneShotTask(TaskType type);

}
}
}
#endif
