#ifndef ALTIMETER_H
#define ALTIMETER_H

namespace Sensors {

namespace Baro {

// addresses of the device
constexpr unsigned char MS561101BA_ADDR_WRITE = 0xEE;   //CBR=1 0x76 I2C address when CSB is connected to HIGH (VCC)
constexpr unsigned char MS561101BA_ADDR_READ  = 0xEF;   //CBR=1 0x76 I2C address when CSB is connected to HIGH (VCC)

// registers of the device
constexpr unsigned char MS561101BA_D1 = 0x40;
constexpr unsigned char MS561101BA_D2 = 0x50;
constexpr unsigned char MS561101BA_RESET = 0x1E;

// D1 and D2 result size (bytes)
constexpr unsigned char MS561101BA_D1D2_SIZE = 3;

// OSR (Over Sampling Ratio) constants
constexpr unsigned char MS561101BA_OSR_256 = 0x00;
constexpr unsigned char MS561101BA_OSR_512 = 0x02;
constexpr unsigned char MS561101BA_OSR_1024 = 0x04;
constexpr unsigned char MS561101BA_OSR_2048 = 0x06;
constexpr unsigned char MS561101BA_OSR_4096 = 0x08;

constexpr unsigned char MS561101BA_PROM_BASE_ADDR = 0xA2; // by adding ints from 0 to 6 we can read all the prom configuration values. 
// C1 will be at 0xA2 and all the subsequent are multiples of 2
constexpr unsigned char MS561101BA_PROM_REG_COUNT = 6; // number of registers in the PROM
constexpr unsigned char MS561101BA_PROM_REG_SIZE = 2; // size in bytes of a prom registry.

void init();
void startTemperatureConversion(unsigned char OSR);
void startPressureConversion(unsigned char OSR);
void updatePressure();
void updateTemperature() __attribute__ ((noinline));

}

}

#endif