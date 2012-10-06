#include "uart.h"
#include <system/devices.hpp>

namespace System {
namespace Bus {
namespace UART {

const unsigned int UART_TX_BUFFER_LENGTH = 32;

enum IoType { Read, Write };

char read() {
    return *DEV_UART_RX;
}

/* TODO: Comment all this */
void write_waiting(char x) {
    while(*DEV_UART_TX == 0) { asm("nop");  }
    *DEV_UART_TX = x;
}

bool write(char x) {
    if(*DEV_UART_TX == 0) return false;
    *DEV_UART_TX = x;
    return true;
}

static void write_loop(const char* x, unsigned int size = 0) {
    unsigned int counter = 0;
    while(*x) {
	if(counter < UART_TX_BUFFER_LENGTH) {
	    if(size > 0) {
		if(counter < size) *DEV_UART_TX = *x++;
		else break;
	    } else *DEV_UART_TX = *x++;
	    counter++;
	} else break;
    }
}

/* if size > 0, write "size" bytes, otherwise write until '\0'
    occur in the string, all in all no more than UART buffer can accept */
bool write(const char* x, unsigned int size) {
    if(*DEV_UART_TX == 0) return false;
    write_loop(x, size);
    return true;
}

void write_waiting(const char* x, unsigned int size) {
    unsigned int counter = 0;
    while(*DEV_UART_TX == 0) { asm("nop"); }
    write_loop(x, size);
}

}
}
}