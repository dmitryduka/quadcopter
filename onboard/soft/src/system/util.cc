#include "util.h"
#include <common>
#include <system>
#include <sensors>

namespace System {

void delay(unsigned int value) { value >>= 2; do { asm("");  } while (--value);  }

static void winkleds() {
    for(unsigned int i = 0; i < 8; ++i) { *DEV_LEDS = 0xFF & (1 << i); delay(10_ms); }
    for(unsigned int i = 0; i < 9; ++i) { *DEV_LEDS = 0xFF & (0x80 >> i); delay(10_ms); }
}

void init() {
#if DEBUG_INFO_UART == 1
    System::Bus::UART::write_waiting("Initialization ... ");
#endif
    Sensors::Baro::init();
    Sensors::IMU::MPU6050::init();
    Sensors::IMU::HMC5883::init();
    winkleds();
#if DEBUG_INFO_UART == 1
    System::Bus::UART::write_waiting(b32todec(Math::divide((unsigned int)*DEV_RTC, 50000)));
    System::Bus::UART::write_waiting(" ms\n");
#endif
}

}

