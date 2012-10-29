#include "altimeter.h"
#include <system/i2c.h>
#include <system/uart.h>
#include <system/util.h>
#include <system/registry.hpp>
#include <common>

namespace Sensors {
namespace Baro {

#define SR System::Registry

unsigned int _C[MS561101BA_PROM_REG_COUNT];
long long int dT, OFF2, SENS2;

namespace I2C = System::Bus::I2C;
namespace UART = System::Bus::UART;

void init() {
    /* reset the sensor */
    I2C::start();
    I2C::write(MS561101BA_ADDR_WRITE);
    I2C::write(MS561101BA_RESET);
    I2C::stop();

    /* wait while MS561101BA reloads PROM into its internal register */
    System::delay(3_ms);

    /* read calibration data from PROM */
    for (int i = 0; i < MS561101BA_PROM_REG_COUNT; ++i) {
	I2C::start();
	I2C::write(MS561101BA_ADDR_WRITE);
	I2C::write(MS561101BA_PROM_BASE_ADDR + (i * MS561101BA_PROM_REG_SIZE));
        I2C::stop();

	I2C::start();
	I2C::write(MS561101BA_ADDR_READ);
        _C[i] = ((unsigned short int)I2C::read() << 8) | I2C::read();
        I2C::stop();
    }
}

static void startConversion(unsigned char addr, unsigned char OSR) {
    I2C::start();
    I2C::write(MS561101BA_ADDR_WRITE);
    I2C::write(addr | OSR);
    I2C::stop();
}

void startPressureConversion(unsigned char OSR) {  startConversion(MS561101BA_D1, OSR); }
void startTemperatureConversion(unsigned char OSR) {  startConversion(MS561101BA_D2, OSR); }

static unsigned int updateValue() {
    I2C::start();
    I2C::write(MS561101BA_ADDR_WRITE);
    I2C::write(0);
    I2C::stop();

    I2C::start();
    I2C::write(MS561101BA_ADDR_READ);
    unsigned int value = ((unsigned int)I2C::read() << 16) |
			((unsigned int)I2C::read() << 8) |
			((unsigned int)I2C::write((char)0xFF));
    I2C::stop();
    return value;
}

/* MS561101BA manual, page 7 */
void updateTemperature() { 
    long long int T = updateValue();
    dT = T - (((unsigned int)_C[4]) << 8); // difference between raw and reference temp.
    long long int TEMP = 2000 + ((dT * _C[5]) / (1 << 23)); // actual temperature in millicentigrades
    /* Second order temp. compensation */
    long long int T2 = 0;
    /* T < 20 */
    if(dT < 0) {
	T2 = (dT * dT) >> 31;
	const unsigned int t = TEMP - 2000;
	const unsigned int t2 = 5 * t * t;
	OFF2 = t >> 1;
	SENS2 = t2 >> 2;
    } else {
	OFF2 = 0;
	SENS2 = 0;
    }
    TEMP = TEMP - T2;
    /* T < -15 */
    if(TEMP < -1500) {
	const unsigned int t = TEMP + 1500;
	const unsigned int tt = t * t;
	OFF2 += 7 * tt;
	SENS2 += (11 * tt) >> 1;
    }
    SR::set(SR::TEMPERATURE, TEMP);
}

/* MS561101BA manual, page 7 */
void updatePressure() { 
    int P = updateValue();
    long long int off = ((long long int)_C[1] << 16) + ((_C[3] * dT) >> 7) - OFF2;
    long long int sens = ((long long int)_C[0] << 15) + ((_C[2] * dT) >> 8) - SENS2;
    P = ((((sens * P) >> 21) - off) >> 15);
    SR::set(SR::PRESSURE, P);
}

}
}
