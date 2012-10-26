#ifndef I2C_H
#define I2C_H

namespace System {
namespace Bus {
namespace I2C {

void start();
void stop();

enum IoType { Read, Write };

unsigned int io(unsigned char, IoType);
unsigned char read();
unsigned char write(unsigned char);

}
}
}

#endif