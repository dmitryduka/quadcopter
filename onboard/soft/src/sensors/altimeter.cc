#include "altimeter.h"
#include <common>
#include <system/i2c.h>
#include <system/util.h>
#include <system/registry.hpp>

namespace Sensors {
namespace Baro {

#define SR System::Registry

unsigned short int _C[MS561101BA_PROM_REG_COUNT];
int dT;
namespace I2C = System::Bus::I2C;

void init() {
    /* reset the sensor */
    I2C::start();
    I2C::write(MS561101BA_ADDRESS);
    I2C::write(MS561101BA_RESET);
    I2C::stop();
    /* wait while MS561101BA reloads PROM into its internal register */
    System::delay(3_ms);

    /* read calibration data from PROM */
    for (int i = 0; i < MS561101BA_PROM_REG_COUNT; ++i) {
	I2C::start();
	I2C::write(MS561101BA_ADDRESS);
	I2C::write(MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE));
	I2C::stop();

	I2C::start();
	I2C::write(MS561101BA_ADDRESS);
        _C[i] = ((unsigned short int)I2C::read() << 8) | I2C::read();
        I2C::stop();
    }
}

static void startConversion(unsigned char addr, unsigned char OSR) {
    I2C::start();
    I2C::write(MS561101BA_ADDRESS);
    I2C::write(addr | OSR);
    I2C::stop();
}

void startPressureConversion(unsigned char OSR) {  startConversion(MS561101BA_D1, OSR); }
void startTemperatureConversion(unsigned char OSR) {  startConversion(MS561101BA_D2, OSR); }

static unsigned int updateValue() {
    I2C::start();
    I2C::write(MS561101BA_ADDRESS);
    I2C::write(0);
    I2C::stop();

    I2C::start();
    I2C::write(MS561101BA_ADDRESS);
    unsigned int value = ((unsigned int)I2C::read() << 16) + 
			((unsigned int)I2C::read() << 8) +
			((unsigned int)I2C::read());
    I2C::stop();
    return value;
}

/* MS561101BA manual, page 7 */
void updateTemperature() { 
    int T = updateValue();
    dT = T - (((int)_C[4]) << 8); // difference between raw and reference temp.
    int TEMP = 2000 + ((dT * _C[5]) >> 23); // actual temperature in millicentigrades
    SR::set(SR::TEMPERATURE, TEMP);
}

/* MS561101BA manual, page 7 */
void updatePressure() { 
    int P = updateValue();
    long long int off = ((long long int)_C[1] << 16) + ((_C[3] * dT) >> 7);
    long long int sens = ((long long int)_C[0] << 15) + ((_C[2] * dT) >> 8);
    P = ((((sens * P) >> 21) - off) >> 15);
    SR::set(SR::PRESSURE, P);
}

}
}
