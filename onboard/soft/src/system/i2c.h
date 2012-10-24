#ifndef I2C_H
#define I2C_H

namespace System {
namespace Bus {
namespace I2C {

void start();
void stop();

char read();
char write(char);

}
}
}

#endif