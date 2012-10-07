#include "xbee.h"
#include <common/ct-utility.hpp>
#include <system/bus/uart/uart.h>
#include <system/devices.hpp>
#include <system/util.h>
#include <radio/messages/console.h>

namespace Radio {

namespace Digital {

XBeeReadIdleTask::XBeeReadIdleTask() : bytesSoFar(0), handler{0, 0} {}
void XBeeReadIdleTask::start() {
    bool ok = false;
    if(System::Bus::UART::can_read()) {
	char ch = System::Bus::UART::read();
	message_buffer[bytesSoFar++] = ch;
	if(ch == '\r' || ch == '\n') {
	    message_buffer[bytesSoFar] = 0;
	    bytesSoFar = 0;
	    ok = true;
	}
    }
    if(ok) parseConsoleMessage(message_buffer);
    /* TODO: poll uart, exit if unavailable */
}

}

}