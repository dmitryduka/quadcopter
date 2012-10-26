#include "altimeter.h"
#include <system>

namespace Sensors {
namespace Baro {

void init() {
    pressure = 0;
}

void startConversion(unsigned char OSR) {
}

unsigned int getPressure() {
    return pressure;
}

}
}
