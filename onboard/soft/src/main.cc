#include <common>
#include <system>
#include <control>

int main() {
    System::init();
    Tasks::TaskScheduler scheduler;

    ADD_IDLE_TASK(scheduler, XBeeReadIdleTask);
    ADD_CONTINUOUS_TASK(scheduler, MARGTask, 200_hz);
    ADD_CONTINUOUS_TASK(scheduler, TelemetryTask, 5_hz);
    ADD_CONTINUOUS_TASK(scheduler, AltitudeTask, 100_hz);

    /* Forever */
    scheduler.start();
    while(1) { System::delay(10); }
}

//=====================================================//
