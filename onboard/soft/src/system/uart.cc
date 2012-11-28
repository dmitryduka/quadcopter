#include "uart.h"
#include "devices.hpp"
#include <common/string.h>

namespace System {
namespace Bus {
namespace UART {

bool can_read() { return ((*DEV_UART_RX >> 16) != 0); }

char read() {
	char c = *DEV_UART_RX & 0xFF;
	*DEV_UART_RX = 0;
    return c;
}

char read_waiting() {
    while(!can_read()) { asm("nop"); }
    return read();
}

/* TODO: Comment all this */
void write_waiting(const char x) {
    while(TX_BUFFER_LENGTH - *DEV_UART_TX == 0) { asm("nop");  }
    *DEV_UART_TX = x;
}

bool write(const char x) {
    if(*DEV_UART_TX == 0) return false;
    *DEV_UART_TX = x;
    return true;
}

void write_waiting(ustring x) {
	for(int i = 0; i < 8; ++i) {
		char c = (x >> 56);
		if(c) write_waiting(c);
		else break;
		x <<= 8;
	}
}

bool write(const ustring x) {
    return write(reinterpret_cast<const char*>(&x), sizeof(ustring));
}

void write_loop(const char* x, unsigned int size) {
    for(unsigned int i = 0; i < TX_BUFFER_LENGTH; ++i) {
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
    unsigned int len = 0;
    if(size) len = size;
    else len = strlen(x);
    while(TX_BUFFER_LENGTH - *DEV_UART_TX < len) { asm("nop"); }
    write_loop(x, size);
}

}
}
}