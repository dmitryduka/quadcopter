#include "uart.h"
#include <system/devices.hpp>

namespace System {
namespace Bus {
namespace UART {

const unsigned int UART_TX_BUFFER_LENGTH = 32;

bool can_read() { return ((*DEV_UART_RX >> 16) != 0); }

char read() {
	char c = *DEV_UART_RX & 0xFF;
	*DEV_UART_RX = 0;
    return c;
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

void write_loop(const char* x, unsigned int size) {
    unsigned int counter = 0;
    for(int i = 0; i < UART_TX_BUFFER_LENGTH; ++i) {
	if(size) {
	    if(i < size) *DEV_UART_TX = x[i];
	    else break;
	} else {
	    if(x[i]) *DEV_UART_TX = x[i];
	    else break;
	}
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