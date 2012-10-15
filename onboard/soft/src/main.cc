#include <common>
#include <system>
#include <control>

using namespace Pool;

int main() {
    System::init();
    Tasks::TaskScheduler scheduler;

    ADD_IDLE_TASK(scheduler, XBeeReadIdleTask);
    ADD_CONTINUOUS_TASK(scheduler, MARGTask, 100_hz);
    ADD_CONTINUOUS_TASK(scheduler, TelemetryTask, 100_hz);

    /* Forever */
    scheduler.start();
    while(1) { System::delay(10); }
}

//=====================================================//
