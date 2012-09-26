#include "uart.h"
#include <system/devices.hpp>

namespace System {
namespace Bus {
namespace UART {

void start() {
/* TODO: implement */
}

void stop() {
/* TODO: implement */
}

enum IoType { Read, Write };

static int io(char b, IoType io) {
/* TODO: implement */
}

char read() { /* TODO: implement */ }
void write(char x) {/* TODO: implement */ }

}
}
}