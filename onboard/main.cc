#include <sys/types.h>
#include "memmgr.h"
#include "registry.hpp"
#include "ct-utility.hpp"
#include "moving_average.hpp"
#include "devices.hpp"
#include "tasks.hpp"

//===============FUNCTION DEFINITIONS==================//

static void led_startup() {
    for(int i = 0; i < 8; ++i) {
	leds(1 << i);
	delay(2000000);
    }
    for(int i = 0; i < 8; ++i) {
	leds(0x80 >> i);
	delay(2000000);
    }
}

//=================== MAIN==============================//
extern"C" int entry()
{
    led_startup();
    MemoryManagerInit();
    TaskScheduler scheduler;
    SystemRegistry registry;

    scheduler.addTask(new HorizontalStabilizationTask, 50);
    scheduler.addTask(new IMUUpdateTask, 100);
    scheduler.addTask(new ThrottleADCTask, 100);
    scheduler.addTask(new EnginesUpdateTask, 440);

    /* Forever */
    scheduler.start();

    /* This should never happen */
    return 42;
}

//=====================================================//
