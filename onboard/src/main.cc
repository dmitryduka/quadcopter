#include "registry.hpp"
#include "ct-utility.hpp"
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

ThrottleADCTask throttleADCTask;
IMUUpdateTask imuUpdateTask;
StabilizationAndEngineUpdateTask stabilizationAndEngineUpdateTask;
XBeeReadIdleTask xbeeReadIdleTask;

//=================== MAIN==============================//
int main() {
    led_startup();
    mpu6050_init();
    TaskScheduler scheduler;

    scheduler.addTask(&xbeeReadIdleTask);
    scheduler.addTask(&imuUpdateTask, 100hz);
    scheduler.addTask(&throttleADCTask, 50hz);
    scheduler.addTask(&stabilizationAndEngineUpdateTask, 440hz);

    /* Forever */
    scheduler.start();
}

//=====================================================//
