#include <common>
#include <system>
#include <control>

int main() {
	System::init();
    System::Bus::UART::write_waiting(0x3031323334353637L);
	Tasks::TaskScheduler scheduler;

    ADD_IDLE_TASK(scheduler, XBeeReadIdleTask);
    //ADD_IDLE_TASK(scheduler, XBeeReadIdleTask, 100_hz);
    ADD_IDLE_TASK(scheduler, MARGTask);
    //ADD_CONTINUOUS_TASK(scheduler, TelemetryTask, 100_hz);

    /* Forever */
    scheduler.start();
    while(1) { System::delay(10); }
}

//=====================================================//
