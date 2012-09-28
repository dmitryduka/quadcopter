#include <common/ct-utility.hpp>
#include <system/devices.hpp>
#include <sensors/imu/mpu6050.h>
#include "util.h"

namespace System {
namespace Util {

void delay(int value) { do { asm("");  } while (--value);  }

static void winkleds() {
    for(unsigned int i = 0; i < 8; ++i) { *DEV_LEDS = 0xFF & (1 << i); delay(100_ms); }
    for(unsigned int i = 1; i < 8; --i) { *DEV_LEDS = 0xFF & (0x80 >> i); delay(100_ms); }
}

void init() {
    Sensors::IMU::MPU6050::init();
    winkleds();
}

}
}

