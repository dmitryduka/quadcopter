//=====================INCLUDES========================//
#include "sf.h"
//=================ADDRESS DEFINE'S====================//

#define DEFINE_EXTERNAL_DEVICE(NAME, ADDR) volatile int * const NAME = reinterpret_cast<volatile int * const>(ADDR);

DEFINE_EXTERNAL_DEVICE(DEV_JTAG,	0x00000000);
DEFINE_EXTERNAL_DEVICE(DEV_LEDS,	0x00000004);
DEFINE_EXTERNAL_DEVICE(DEV_ADC,		0x00000008);
DEFINE_EXTERNAL_DEVICE(DEV_IMU,		0x0000000C);
DEFINE_EXTERNAL_DEVICE(DEV_RTC,		0x00000010);
DEFINE_EXTERNAL_DEVICE(DEV_UART_TX,	0x00000014);
DEFINE_EXTERNAL_DEVICE(DEV_UART_RX,	0x00000018);


//===============FUNCTION DECLARATIONS=================//

extern "C" void delay(int);
extern "C" int mymain();
static int  user_input();
static void leds(int);
static void usleep(int);
static int adc_read(int);

extern "C" float32 int32_to_float32( int32 );
extern "C" int32 float32_to_int32( float32 );
extern "C" float32 float32_round_to_int( float32 );
extern "C" float32 float32_add( float32, float32 );
extern "C" float32 float32_sub( float32, float32 );
extern "C" float32 float32_mul( float32, float32 );
extern "C" flag float32_eq( float32, float32 );
extern "C" flag float32_le( float32, float32 );
extern "C" flag float32_lt( float32, float32 );
extern "C" flag float32_eq_signaling( float32, float32 );
extern "C" flag float32_le_quiet( float32, float32 );
extern "C" flag float32_lt_quiet( float32, float32 );
extern "C" flag float32_is_signaling_nan( float32 );

//===============FUNCTION DEFINITIONS==================//

static void usleep(int useconds)
{	delay(useconds*10); 
	return;	}

static int user_input()
{	return *DEV_LEDS; }

static void leds(int l)
{	*DEV_LEDS = l;	}

static int adc_read(int ch)
{   int r = -1;
    *DEV_ADC = ch;
    while (r < 0) r = *DEV_ADC;
    return r;	}

static void do_adc()
{	static unsigned char z = 0xC0;
	
	z >>= 2;
	if(z == 0) z = 0xC0;
	leds(z);
	usleep(10000 + 100*adc_read(2)); 

	return; }

static void do_leds()
{	static unsigned char a = 0x10, left = 1;
	
	if(left)
		{	if(a == 0x80) left = 0;
			else	a <<= 1;	}
	else
		{	if(a == 0x01) left = 1;
			else	a >>= 1;	}
		leds(a);
	usleep(10000 * (1 + (0xF & user_input())));

	return; }

static void print_hex(unsigned int a)
{	while(*DEV_UART_TX == 0) {usleep(1000);}
	for(int i = 0; i < 8; ++i)
	{	unsigned char c = ((a >> 28) & 0xF);
		if (c < 10) c += '0';
		else		c += 'A' - 10;
		*DEV_UART_TX = c;
		a <<= 4;
	} 	
	*DEV_UART_TX = '\n'; }

static void print_str(const char * str)
{	while(*DEV_UART_TX == 0) {usleep(1000);}
	for(int i = 0; i < 32; ++i)
	{	if(str[i])	*DEV_UART_TX = str[i];
		else break;	} }

float32 fast_sqrt(float32 x)
{  x  += 127 << 23;
   x >>= 1; 
   return x; }   
//===================MAIN==============================//
const unsigned char lv[] = {0x81, 0x42, 0x24, 0x18};

int main()
{	/*for(int i = 0; i < 4; ++i)
	{	leds(lv[i]);
		usleep(300000); }	*/

	print_str("Hello, a thirty-symbol string!\n");	
	
	int rtc_a = *DEV_RTC;
	float32 a = int32_to_float32(5);
	float32 b = int32_to_float32(6);
	int rtc_b = *DEV_RTC;
	print_str("Cons time: ");
	print_hex(rtc_b - rtc_a);

	rtc_a = *DEV_RTC;
	float32 s = float32_add(a, b );
	rtc_b = *DEV_RTC;
	print_str("Add time: ");
	print_hex(rtc_b - rtc_a);

	rtc_a = *DEV_RTC;
	float32 m = float32_mul(a, b );
	rtc_b = *DEV_RTC;
	print_str("Mul time: ");
	print_hex(rtc_b - rtc_a);
	
	print_str("Add result: "); 	print_hex(s);
	print_str("Mul result: "); 	print_hex(m);

	float32 fsqrt = fast_sqrt(a);
	print_str("Sqrt result: "); print_hex(fsqrt);


	return 42; }

//=====================================================//
