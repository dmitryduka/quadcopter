#ifndef TASK_POOL_H
#define TASK_POOL_H
#include "tasks.h"

namespace System {
namespace Tasking {
namespace Pool {

enum TaskType {
    TASK_TYPE_BEGIN = 0,
    StabilizationAndEngineUpdateTask = 0,
    XBeeReadIdleTask,
    TelemetryTask,
    MARGTask,
    AltitudeTask,
    MagnetometerTask,
    TASK_TYPE_COUNT
};

Task*    const     getTask(TaskType type);

}
}
}
#endif
