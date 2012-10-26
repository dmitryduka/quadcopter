#include "i2c.h"
#include "devices.hpp"

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

unsigned int io(unsigned char b, IoType io) {
    unsigned int x = b;
    if(io == Write) x |= 0x100;
    *DEV_I2C = 0x00020000 | x;
    while (*DEV_I2C < 0);
    return *DEV_I2C;
}

unsigned char read() {  return io(0xFF, Read) & 0xFF; }
unsigned char write(unsigned char x) { return io(x, Write) & 0xFF; }

}
}
}