#include <common>
#include <system>
#include <control>

int main() {
    System::init();

    ADD_IDLE_TASK(XBeeReadIdleTask);
    ADD_CONTINUOUS_TASK(MARGTask, 50_hz);
    ADD_CONTINUOUS_TASK(TelemetryTask, 50_hz);
    ADD_CONTINUOUS_TASK(MagnetometerTask, 70_hz);
    ADD_CONTINUOUS_TASK(AltitudeTask, 100_hz);

    /* Forever */
    Tasks::TaskScheduler::instance().start();
    while(1) { System::delay(10); }
}

//=====================================================//
