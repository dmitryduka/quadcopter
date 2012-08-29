#include "xbee.h"
#include "devices.hpp"

XBeeReadIdleTask::XBeeReadIdleTask() : bytesSoFar(0), handler{0, 0} {}
void XBeeReadIdleTask::start() {
    /* TODO: poll uart, exit if unavailable */
    /* TODO */
    /* read it otherwise */
    char b = uart_read();
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

