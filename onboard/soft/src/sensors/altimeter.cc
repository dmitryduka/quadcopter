#include "altimeter.h"
#include <common>
#include <system/i2c.h>
#include <system/util.h>

namespace Sensors {
namespace Baro {

unsigned short int _C[MS561101BA_PROM_REG_COUNT];
namespace I2C = System::Bus::I2C;

void init() {
    /* reset the sensor */
    I2C::start();
    I2C::write(MS561101BA_ADDR_CSB_LOW);
    I2C::write(MS561101BA_RESET);
    I2C::stop();
    /* wait while MS561101BA reloads PROM into its internal register */
    System::delay(3_ms);

    /* read calibration data from PROM */
    for (int i = 0; i < MS561101BA_PROM_REG_COUNT; ++i) {
	I2C::start();
	I2C::write(MS561101BA_ADDR_CSB_LOW);
	I2C::write(MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE));
	I2C::stop();

	I2C::start();
	I2C::write(MS561101BA_ADDR_CSB_LOW);
        _C[i] = ((unsigned short int)I2C::read() << 8) | I2C::read();
        I2C::stop();
    }
}

void startTemperatureConversion(unsigned char OSR) {
    /* TODO: request sensor to start conversion */
}

void startPressureConversion(unsigned char OSR) {
    /* TODO: request sensor to start conversion */
}

void updatePressure() {
    /* TODO: read values from the sensor and update registry */
    //System::Registry::set(System::Registry::PRESSURE, );
}

void updateTemperature() {
    /* TODO: read values from the sensor and update registry */
    //System::Registry::set(System::Registry::PRESSURE, );
}

}
}
