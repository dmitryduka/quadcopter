#include "magnetometer.h"
#include <common>
#include <system/i2c.h>
#include <system/registry.hpp>

namespace Sensors {
namespace IMU {
namespace HMC5883 {

namespace I2C = System::Bus::I2C;

static char hmc5883_reg_read(unsigned char reg) {
    I2C::start();
    I2C::write(HMC5883_ADDR_WRITE);
    I2C::write(reg);
    I2C::stop();

    I2C::start();
    I2C::write(HMC5883_ADDR_READ);
    char ax  = I2C::write(0xFF) & 0xFF;
    I2C::stop();
    return ax;
}

void init() {
    I2C::start();
    I2C::write(HMC5883_ADDR_WRITE);
    I2C::write(HMC5883_CFGA);
    I2C::write(HMC5883_CFGA_75HZ_8SAMPLES);
    I2C::stop();

    I2C::start();
    I2C::write(HMC5883_ADDR_WRITE);
    I2C::write(HMC5883_MODE);
    I2C::write(HMC5883_MODE_CONTINUOUS);
    I2C::stop();
    System::delay(10_ms);
}

void update() {
    I2C::start();
    I2C::write(HMC5883_ADDR_WRITE);
    I2C::write(HMC5883_DATA_START);
    I2C::stop();

    I2C::start();
    I2C::write(HMC5883_ADDR_READ);
    int mx = I2C::read();
    int mxL = I2C::read();
    int mz = I2C::read();
    int mzL = I2C::read();
    int my = I2C::read();
    int myL = I2C::write(0xFF) & 0xFF;
    mx = Math::sign_extend((mx << 8) | mxL);
    my = Math::sign_extend((my << 8) | myL);
    mz = Math::sign_extend((mz << 8) | mzL);
    I2C::stop();

    System::Registry::set(System::Registry::COMPASS_X, mx);
    System::Registry::set(System::Registry::COMPASS_Y, my);
    System::Registry::set(System::Registry::COMPASS_Z, mz);
}

}
}
}
