#include "ct-utility.hpp"
#include "tasks.h"
#include "task_pool.h"

namespace TP = TaskPool;

static void led_startup() {
    for (int i = 0; i < 8; ++i) {
        leds(1 << i);
        delay(200_ms);
    }
    for (int i = 0; i < 8; ++i) {
        leds(0x80 >> i);
        delay(200_ms);
    }
}

int main() {
    led_startup();
    mpu6050_init();
    TaskScheduler scheduler;

    scheduler.addTask(TP::getIdleTask(TP::TaskType::XBeeReadIdleTask));
    scheduler.addTask(TP::getContinuousTask(TP::TaskType::StabilizationAndEngineUpdateTask), 440_hz);

    /* Forever */
    scheduler.start();
}

//=====================================================//
