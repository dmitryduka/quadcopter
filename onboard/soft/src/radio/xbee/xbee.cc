#include "xbee.h"
#include <system/bus/uart/uart.h>
#include <system/devices.hpp>
#include <radio/messages/console.h>

namespace Radio {

namespace Digital {

XBeeReadIdleTask::XBeeReadIdleTask() : bytesSoFar(0), handler{0, 0} {}
void XBeeReadIdleTask::start() {
    /* Just for testing */
    parseConsoleMessage(message_buffer);
    /* TODO: poll uart, exit if unavailable */
    /* TODO */
    /* read it otherwise */
    char b = System::Bus::UART::read();
    /* Determine message size/handler, because first byte is always message type */
    if(bytesSoFar == 0)
        handler = Messages::handlers[b];
    /* Put the byte to the buffer */
    message_buffer[bytesSoFar++] = b;
    /* check if we have full message yet and call handler if so */
    if(bytesSoFar == handler.size) {
        handler.handler(message_buffer);
        bytesSoFar = 0;
        handler = {0, 0};
    }
}

}

}