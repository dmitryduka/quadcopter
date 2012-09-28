#include "i2c.h"
#include <system/devices.hpp>

namespace System {
namespace Bus {
namespace I2C {

void start() {
    *DEV_I2C = 0x000400FF;
    while (*DEV_I2C < 0);
}

void stop() {
    *DEV_I2C = 0x000100FF;
    while (*DEV_I2C < 0);
}

enum IoType { Read, Write };

static int io(char b, IoType io) {
    int x = b;
    if(io == Write) x |= 0x100;
    *DEV_I2C = 0x00020000 | b;
    while (*DEV_I2C < 0);
    return *DEV_I2C;
}

char read() {  return io(0xFF, Read) & 0xFF; }
void write(char x) { io(x, Write); }

}
}
}