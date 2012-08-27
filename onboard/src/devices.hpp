#ifndef DEVICES_HPP
#define DEVICES_HPP

#define DEFINE_EXTERNAL_DEVICE(NAME, ADDR) volatile int * const NAME = reinterpret_cast<volatile int * const>(ADDR);

/* Accelerometer */
DEFINE_EXTERNAL_DEVICE(ACC_DATA_X,	0x00000000);
DEFINE_EXTERNAL_DEVICE(ACC_DATA_Y,	0x00000004);
DEFINE_EXTERNAL_DEVICE(ACC_DATA_Z,	0x00000008);

/* ADC */
DEFINE_EXTERNAL_DEVICE(ADC_ADDR,	0x0000000C);

/* Engines */
DEFINE_EXTERNAL_DEVICE(ENGINES_13_ADDR,	0x00000010);
DEFINE_EXTERNAL_DEVICE(ENGINES_24_ADDR,	0x00000014);

/* LED */
DEFINE_EXTERNAL_DEVICE(LED_ADDR,	0x00000018);

/* I2C */
DEFINE_EXTERNAL_DEVICE(I2C_ADDR,	0x0000001C);

/* EVENTS */
DEFINE_EXTERNAL_DEVICE(EVENTS_ADDR,	0x00000020);

/* RTC */
DEFINE_EXTERNAL_DEVICE(RTC_ADDR,	0x00000030);

/* UART1 */
DEFINE_EXTERNAL_DEVICE(UART1_ADDR,	0x00000040);

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
    *LED_ADDR = l & 0xFF;
}

static void delay(int value) {
    do {
        asm("");
    }
    while (value--);
}

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
    while (r < 0) r = *ADC_ADDR;
    return r;
}

static void wait4event()
{
    //wait for event
    while (*EVENTS_ADDR == 0) { }
    *EVENTS_ADDR = 1;
}

/* Read 2 words for RTC */
static unsigned int RTC() {
    return *RTC_ADDR;
}

static void i2c_start()
{
    *I2C_ADDR = 0x000400FF;
    while (*I2C_ADDR < 0);
}

static void i2c_stop() {
    *I2C_ADDR = 0x000100FF;
    while (*I2C_ADDR < 0);
}

static int i2c_io(int b) {
    b &= 0x1FF;
    *I2C_ADDR = 0x00020000 | b;
    while (*I2C_ADDR < 0);
    return *I2C_ADDR;
}

static void mpu6050_write(int reg, int byte)
{
    i2c_start();
    i2c_io(MPU6050_ADDR);
    i2c_io(0x100 | reg);
    i2c_io(0x100 | byte);
    i2c_stop();
}

static int sign_extend(int halfword)
{
    return  ((halfword & 0xFFFF) >> 15) ? (0xFFFF0000 | halfword) : halfword;
}

static void uart_write(char x) {
    // TODO: implement this correctly
    while (*UART1_ADDR == 0) { }
    *UART1_ADDR = x;
}

static char uart_read() {
    // TODO: implement this correctly
    char x;
    while (x < 0) { x = *UART1_ADDR; }
    return x;
}

static void mpu6050_init() {
    /* TODO: no magic values */
    mpu6050_write(0x1A, 0x03); //Low-pass ON
    mpu6050_write(0x1B, 0x18); //GYRO_CONFIG: +-2000 dps range
    mpu6050_write(0x1C, 0x18); //ACC_CONFIG: +-16g range
    mpu6050_write(0x6B, 0x00); //Sleep disable
    delay(40000);
}

#endif
