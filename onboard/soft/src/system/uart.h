#ifndef UART_H
#define UART_H

#include "types.h"

namespace System {
namespace Bus {
namespace UART {

bool can_read();
char read();
bool write(const char);
bool write(const ustring);
void write_waiting(const char);
void write_waiting(ustring);

bool write(const char*, unsigned int size = 0);
void write_loop(const char*, unsigned int size = 0);
void write_waiting(const char*, unsigned int size = 0);

}
}
}

#endif