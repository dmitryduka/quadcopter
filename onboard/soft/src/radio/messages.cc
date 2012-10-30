#include "messages.hpp"
#include "console.h"

namespace Radio {

namespace Messages {

void defaultHandler(char*) {}

void consoleHandler(char* str) {
#if CONSOLE_ENABLED == 1
    parseConsoleMessage(str);
#else
    System::Bus::UART::write_waiting(":|\n");
#endif
}

}

}
