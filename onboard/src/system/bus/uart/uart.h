#ifndef I2C_H
#define I2C_H

namespace System {
namespace Bus {
namespace UART {

void start();
void stop();

char read();
void write(char);

}
}
}

#endif