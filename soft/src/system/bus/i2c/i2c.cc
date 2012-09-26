#include "i2c.h"
#include <system/devices.hpp>

namespace System {
namespace Bus {
namespace I2C {

void start() {
    *I2C_ADDR = 0x000400FF;
    while (*I2C_ADDR < 0);
}

void stop() {
    *I2C_ADDR = 0x000100FF;
    while (*I2C_ADDR < 0);
}

enum IoType { Read, Write };

static int io(char b, IoType io) {
    int x = b;
    if(io == Write) x |= 0x100;
    *I2C_ADDR = 0x00020000 | b;
    while (*I2C_ADDR < 0);
    return *I2C_ADDR;
}

char read() {  return io(0xFF, Read) & 0xFF; }
void write(char x) { io(x, Write); }

}
}
}