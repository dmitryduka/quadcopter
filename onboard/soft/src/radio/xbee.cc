#include "xbee.h"
#include <common>
#include <system>
#include <radio>

namespace Radio {

namespace Digital {

XBeeReadIdleTask::XBeeReadIdleTask() : bytesSoFar(0), current_message{0, 0} {}
void XBeeReadIdleTask::start() {
    /* If we have something to read from UART, do it, otherwise return */
    if(System::Bus::UART::can_read()) {
	unsigned char ch = System::Bus::UART::read();
	/* awaiting for the first character that could be interpreted as a message header/type */
	if(!current_message.handler) {
	    if(ch == asIntegral<int, Radio::Messages::To>(Radio::Messages::To::CONSOLE_REQUEST)) {
		current_message.size = 0;
		current_message.handler = Radio::Messages::consoleHandler;
	    } else {
		if(ch > 0 && ch < asIntegral<int, Radio::Messages::To>(Radio::Messages::To::BINARY_MESSAGES_COUNT)) {
		    const Radio::Messages::EntryType entry = Radio::Messages::handlers[ch];
		    current_message.size = entry.size;
		    current_message.handler = entry.handler;
		}
	    }
	} else {
	    bool call_handler = false, reset_entry = false;
	    /* Do not overflow the message buffer */
	    if(bytesSoFar < Radio::Messages::MAX_MESSAGE_LENGTH) message_buffer[bytesSoFar++] = ch;
	    /* Otherwise discard buffer */
	    else reset_entry = true;
	    if(current_message.handler == Radio::Messages::consoleHandler) {
		/* Await for \r and call console handler */
		if(ch == '\r' || ch == '\n') {
		    message_buffer[bytesSoFar] = '\0';
		    call_handler = true;
		}
	    } else {
		/* other message, wait while receive enough bytes and then call message handler */
		if(bytesSoFar == current_message.size) call_handler = true;
	    }
	    if(call_handler) {
		    current_message.handler(message_buffer);
		    reset_entry = true;
	    }
	    if(reset_entry) {
		    bytesSoFar = 0;
		    current_message = {0, 0};
	    }
	}
    }
}

}

}