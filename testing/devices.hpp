#ifndef DEVICES_HPP
#define DEVICES_HPP

const int ENGINE_THRUST_RANGE_LOW = 0;
const int ENGINE_THRUST_RANGE_HIGH = 512;


/* System settings */
const unsigned int CPU_FREQUENCY_HZ = 50000000;
const unsigned int MAX_TASK_INTERVAL_TICKS = CPU_FREQUENCY_HZ * 60; // once in a minute

const unsigned int MPU6050_ADDR = 0x1D0;


//===============FUNCTION DEFINITIONS==================//

/* Clamp value */
inline static int clamp(int x, int low, int high) {
    return (x >= low && x <= high) ? x : ((x < low) ? low : high);
}

static void leds(int l) {
}

static void delay(int value) {
    do {
        asm("");
    }
    while (value--);
}

static void eng_ctrl(int v2, int v1, volatile int* ENGINE)
{
}

static int adc_read(int ch)
{
}

static void wait4event()
{
}

/* Read 2 words for RTC */
static unsigned int RTC() {
    static unsigned int rtc = 0;
    return rtc++;
}

static void i2c_start()
{
}

static void i2c_stop() {
}

static int i2c_io(int b) {
}

static void mpu6050_write(int reg, int byte)
{
}

static int sign_extend(int halfword)
{
    return  ((halfword & 0xFFFF) >> 15) ? (0xFFFF0000 | halfword) : halfword;
}

static void uart_write(char x) {
}

static char uart_read() {
}

static void mpu6050_init() {
}

#endif
