#include <common>
#include <system>
#include <control>

int main() {
    System::init();
    Tasks::TaskScheduler scheduler;

    ADD_IDLE_TASK(scheduler, XBeeReadIdleTask);
    ADD_CONTINUOUS_TASK(scheduler, MARGTask, 70_hz);
    ADD_CONTINUOUS_TASK(scheduler, TelemetryTask, 50_hz);
    ADD_CONTINUOUS_TASK(scheduler, MagnetometerTask, 70_hz);
    ADD_CONTINUOUS_TASK(scheduler, AltitudeTask, 100_hz);

    /* Forever */
    scheduler.start();
    while(1) { System::delay(10); }
}

//=====================================================//
