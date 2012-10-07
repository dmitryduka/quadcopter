#ifndef I2C_H
#define I2C_H

namespace System {
namespace Bus {
namespace UART {

bool can_read();
char read();
bool write(char);
void write_waiting(char);

bool write(const char*, unsigned int size = 0);
void write_loop(const char*, unsigned int size = 0);
void write_waiting(const char*, unsigned int size = 0);

}
}
}

#endif