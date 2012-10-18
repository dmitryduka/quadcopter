#include "messages.hpp"
#include "console.h"

namespace Radio {

namespace Messages {

void defaultHandler(char*) {}

void consoleHandler(char* str) {
    parseConsoleMessage(str);
}

}

}
