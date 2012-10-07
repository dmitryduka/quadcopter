#ifndef DEVICES_HPP
#define DEVICES_HPP

#define DEFINE_EXTERNAL_DEVICE(NAME, ADDR) volatile int * const NAME = reinterpret_cast<volatile int * const>(ADDR);

DEFINE_EXTERNAL_DEVICE(DEV_JTAG,	0x00000000);
DEFINE_EXTERNAL_DEVICE(DEV_LEDS,	0x00000004);
DEFINE_EXTERNAL_DEVICE(DEV_ADC,		0x00000008);
DEFINE_EXTERNAL_DEVICE(DEV_IMU,		0x0000000C);
DEFINE_EXTERNAL_DEVICE(DEV_I2C,		0x0000000C);
DEFINE_EXTERNAL_DEVICE(DEV_RTC,		0x00000010);
DEFINE_EXTERNAL_DEVICE(DEV_UART_TX,	0x00000014);
DEFINE_EXTERNAL_DEVICE(DEV_UART_RX,	0x00000018);
DEFINE_EXTERNAL_DEVICE(DEV_ENG_13,	0x0000001C);
DEFINE_EXTERNAL_DEVICE(DEV_ENG_24,	0x00000020);



/* System settings */
const unsigned int CPU_FREQUENCY_HZ = 50000000;

#endif