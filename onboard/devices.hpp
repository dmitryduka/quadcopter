#ifndef DEVICES_HPP
#define DEVICES_HPP

#include "ct-utility.hpp"

/* Accelerometer */
DEFINE_EXTERNAL_DEVICE(ACC_DATA_X,	0x00000000);
DEFINE_EXTERNAL_DEVICE(ACC_DATA_Y,	0x00000004);
DEFINE_EXTERNAL_DEVICE(ACC_DATA_Z,	0x00000008);
DEFINE_EXTERNAL_DEVICE(ADC_ADDR,	0x0000000C);

/* Engines */
DEFINE_EXTERNAL_DEVICE(ENGINES_13_ADDR,	0x00000010);
DEFINE_EXTERNAL_DEVICE(ENGINES_24_ADDR,	0x00000014);

/* LED */
DEFINE_EXTERNAL_DEVICE(LED_ADDR,	0x00000018);

/* EVENTS */
DEFINE_EXTERNAL_DEVICE(EVENTS_ADDR,	0x00000020);

/* RTC */
DEFINE_EXTERNAL_DEVICE(RTC_ADDR,	0x00000030);

const int ENGINE_THRUST_RANGE_LOW = 0;
const int ENGINE_THRUST_RANGE_HIGH = 512;

/* System settings */
const int CPU_FREQUENCY_HZ = 100000000;

//===============FUNCTION DEFINITIONS==================//

static void leds(int l) { *LED_ADDR = l & 0xFF; }

static void delay(int value) {  do { asm(""); value--;  } while(value); }

static void eng_ctrl(int v2, int v1, volatile int* ENGINE)
{
    v1 = clamp(v1, ENGINE_THRUST_RANGE_LOW, ENGINE_THRUST_RANGE_HIGH);
    v2 = clamp(v2, ENGINE_THRUST_RANGE_LOW, ENGINE_THRUST_RANGE_HIGH);
    *ENGINE = ((v2 << 16) | v1);
}

static int adc_read(int ch)
{
    int r = -1;
    *ADC_ADDR = ch;
    while(r < 0) r = *ADC_ADDR;
    return r;
}

static void wait4event()
{
    //wait for event
    while(*EVENTS_ADDR == 0) { }
    *EVENTS_ADDR = 1;
    return;
}

/* Read 2 words for RTC */
static long long RTC() { return *(long long *)RTC_ADDR; }

#endif
