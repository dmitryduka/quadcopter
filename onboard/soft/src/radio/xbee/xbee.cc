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
    /* In case it is console message accumulate bytes into ConsoleRequest::command until '\r'/'\n' is read or buffer is full.
	Otherwise it is a message with predefined length so read until we have full message and call it's handler
    */

    /* If we have something to read from UART, do it, otherwise return */
    if(System::Bus::UART::can_read()) {
	if(1/* awaiting for the first character that could be interpretable as a message header/type */) {
	} else {
	    if(1/* console message */) {
	    } else {
	    /* other message */
	    }
	}
    }
}

}

}