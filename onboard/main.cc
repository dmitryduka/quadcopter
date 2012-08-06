//=================ADDRESS DEFINE'S====================//

#define DEFINE_EXTERNAL_DEVICE(NAME, ADDR) volatile int * const NAME = reinterpret_cast<volatile int * const>(ADDR);
#define DEFINE_ARRAY(TYPE, NAME, BASE_ADDR) volatile TYPE * const NAME = reinterpret_cast<volatile TYPE * const>(0xFFFF##BASE_ADDR);

DEFINE_EXTERNAL_DEVICE(ACC_DATA_X,	0x00000000);
DEFINE_EXTERNAL_DEVICE(ACC_DATA_Y,	0x00000004);
DEFINE_EXTERNAL_DEVICE(ACC_DATA_Z,	0x00000008);
DEFINE_EXTERNAL_DEVICE(ADC_ADDR,	0x0000000C);

DEFINE_EXTERNAL_DEVICE(ENGINES_13_ADDR,	0x00000010);
DEFINE_EXTERNAL_DEVICE(ENGINES_24_ADDR,	0x00000014);
DEFINE_EXTERNAL_DEVICE(LED_ADDR,	0x00000018);

DEFINE_EXTERNAL_DEVICE(EVENTS_ADDR,	0x00000020);

const int ENGINE_THRUST_RANGE_LOW = 0;
const int ENGINE_THRUST_RANGE_HIGH = 512;

/* PID settings */
const int PID_STRENGTH = 1024; // 1.0
const int Kp = 128; // 0.25
const int Ki = 64; // 8/1024
const int ITERM_MAX = 20; // 2%
const int ITERM_RANGE = 200; // ~ +-10 degrees range
const int Kd = 4096; // 2.0
const int DIFFERENTIAL_TIME_CONSTANT = 30; // ~ 0.015s
const int K_DENOMINATOR = 1024; /* Should be power of 2 */

//===============FUNCTION DEFINITIONS==================//
typedef unsigned int size_t;

/* Compile time log (to compute shift) */
template <size_t N, size_t base=2> struct log_ { enum { value = 1 + log_<N / base, base>::value }; };
template <size_t base> struct log_<1, base> { enum { value = 0 }; };
template <size_t base> struct log_<0, base> { enum { value = 0 }; };

/* N is window width, should be power of 2 */
template<int N>
class moving_average
{
public:
    moving_average() : num_samples_(0), total_(0) { 
	for(int i = 0; i < N; ++i) samples_[i] = 0;
    }

    moving_average& operator()(int sample) {
        int& oldest = samples_[num_samples_++];
        if(num_samples_ == N) num_samples_ = 0;
        total_ += sample - oldest;
        oldest = sample;
        return *this;
    }

    operator int() const { return total_ >> log_<N>::value; }

private:
    int samples_[N];
    int num_samples_;
    int total_;
};

static void leds(int l) { *LED_ADDR = l & 0xFF; }

static void delay(int value) {  do { asm(""); value--;  } while(value); }

static int abs(int x) { if(x < 0) return -x; return x; }

static int clamp(int value, int low, int high) {
    return (value >= low && value <= high) ? 
	    value : 
	    ((value < low) ? 
			low : 
			high);
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

/* Q should be power of 2 */
template<int P, int Q> inline int scale(int x) { return (x * P) >> log_<Q>::value; }

static int off(int x) { return 0; }

class base {
public:
    virtual int f() { return 0; }
};

class foo : public base {
public:
    virtual int f() { return -1; }
};

void test(base& b) { b.f(); }

//===================MAIN==============================//

int main()
{
    leds(0x81); delay(8333333);
    leds(0x42); delay(8333333);
    leds(0x24); delay(8333333);
    leds(0x18); delay(8333333);

    foo f;
    test(f);

    int xo = 0,
	yo = 0,
	ix = 0,
	iy = 0,
	dx = 0,
	dy = 0,
	frame = 0;
    moving_average<16> accelerometer_x_lp, accelerometer_y_lp;
    while(1)
    {
	wait4event();

	/* Get current thrust */
	int thrust = adc_read(2);
	leds(thrust >> 4);
	thrust >>= 2;

	/* Get current accelerometer data */
	int cx = *ACC_DATA_X - *ACC_DATA_Y;
	int cy = *ACC_DATA_X + *ACC_DATA_Y;

	/* Filter accelerometer with simple moving average filter */
	int x = accelerometer_x_lp(cx);
	int y = accelerometer_y_lp(cy);

	/* P term */
	int px = x;
	int py = y;

	/* I term */
	/* Accumulate error */
	if(abs(x) < ITERM_RANGE) ix += x;
	/* Reset I term if error is 0 */
	else ix = 0;
	/* Accumulate error */
	if(abs(y) < ITERM_RANGE) iy += y;
	/* Reset I term if error is 0 */
	else iy = 0;

	/* D term, do this once in DIFFERENTIAL_TIME_CONSTANT steps */
	if(frame == DIFFERENTIAL_TIME_CONSTANT) {
	    dx = xo - x;
	    dy = yo - y;
	    xo = x;
	    yo = y;
	    frame = 0;
	} else frame++;
	

	int PIDx = 0; 
	int PIDy = 0; 
	
	/* Do not perform any PID controlling if throttle is below 15% */
	if(thrust > 150) {
	    PIDx = scale<PID_STRENGTH, K_DENOMINATOR>(scale<Kp, K_DENOMINATOR>(px) 
						    + clamp(scale<Ki, K_DENOMINATOR>(ix), -ITERM_MAX, ITERM_MAX) 
						    - scale<Kd, K_DENOMINATOR>(dx));

	    PIDy = scale<PID_STRENGTH, K_DENOMINATOR>(scale<Kp, K_DENOMINATOR>(py) 
						    + clamp(scale<Ki, K_DENOMINATOR>(iy), -ITERM_MAX, ITERM_MAX) 
						    - scale<Kd, K_DENOMINATOR>(dy));
	}

	eng_ctrl(thrust + PIDx, thrust - PIDx, ENGINES_13_ADDR);
	eng_ctrl(thrust - PIDy, thrust + PIDy, ENGINES_24_ADDR);
    }
    
    /* This should never happen */
    return 42;
}

//=====================================================//
