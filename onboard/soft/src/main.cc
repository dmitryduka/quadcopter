#include <common>
#include <system>
#include <control>

int main() {
    System::init();
    Tasks::TaskScheduler scheduler;

    ADD_IDLE_TASK(scheduler, XBeeReadIdleTask);
    ADD_CONTINUOUS_TASK(scheduler, MARGTask, 100_hz);
    ADD_CONTINUOUS_TASK(scheduler, TelemetryTask, 10_hz);
    ADD_CONTINUOUS_TASK(scheduler, AltitudeTask, 25_hz);

    /* Forever */
    scheduler.start();
    while(1) { System::delay(10); }
}

//=====================================================//
