#include <sys/types.h>
#include "memmgr.h"
#include "registry.hpp"
#include "ct-utility.hpp"
#include "moving_average.hpp"
#include "devices.hpp"
#include "tasks.hpp"
#include "messages.hpp"

//===============FUNCTION DEFINITIONS==================//

static void led_startup() {
    for (int i = 0; i < 8; ++i) {
        leds(1 << i);
        delay(200ms);
    }
    for (int i = 0; i < 8; ++i) {
        leds(0x80 >> i);
        delay(200ms);
    }
}

//=================== MAIN==============================//
int main() {
    Messages::handlers[*reinterpret_cast<int*>(0x0)]();
    led_startup();
    mpu6050_init();
    TaskScheduler scheduler;

    scheduler.addTask(new StabilizationAndEngineUpdateTask, 440hz);

    /* Forever */
    scheduler.start();
}

//=====================================================//
