template<typename T> T max(T a, T b) { return a < b ? b : a; }

unsigned int abs(int x) { return x < 0 ? -x : x; }

// Approximates sqrt(x^2 + y^2) with a maximum error of 4%
// Uses two squares shifted by 45 dg
unsigned int fastnorm2(int x, int y)
{
    x = abs(x); y = abs(y);
    // (1 + sqrt(4 - 2 * qSqrt(2))) / 2 = (68235 / 2 ^ 16)
    // sqrt(1 / 2) = (46340 / 2 ^ 16) 
    const unsigned int metricOne = max(x, y);
    const unsigned int metricTwo = (46340 * (x + y)) >> 16;
    return (68235 * max(metricOne, metricTwo)) >> 16;
}

unsigned int fastnorm3(int x, int y, int z) { return fastnorm2(y, fastnorm2(x, z)); }


unsigned int isqrt(unsigned int x) {
    unsigned int op = x;
    unsigned int res = 0;
    unsigned int one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type

    // "one" starts at the highest power of four <= than the argument.
    while (one > op) {
	one >>= 2;
    }
    while (one != 0) {
	if (op >= res + one) {
	    op = op - (res + one);
	    res = res +  2 * one;
	}
	res >>= 1;
	one >>= 2;
    }
    return res;
}

int divide(int num, int denom)
{
    int a = 0, b = 0, i = 31;
    /* Work from leftmost to rightmost bit in numerator */
    while(i >= 0) {
	/* appends one bit from numerator to a */
	a = (a << 1) + ((num & (1 << i)) >> i); 
	b = b << 1;
	if (a >= denom) {
	    a -= denom;
	    b++;
	}
	i--;
    }
    return b;
}

// Requirements:
// P >= 0
// A > 0
// P, A ~ [0 ; 2048] (?)
// P < A
// P/A ~ [0; 1]
// 
static unsigned char asin_lut[128] = { 0, 1, 3, 4, 5, 6, 8, 9, 10, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23, 24, 26, 27, 28, 30, 31, 32, 33, 35, 36, 37, 39, 40, 41, 43, 44, 45, 47, 48, 49, 51, 52, 53, 55, 56, 57, 59, 60, 62, 63, 64, 66, 67, 68, 70, 71, 73, 74, 76, 77, 78, 80, 81, 83, 84, 86, 87, 89, 90, 92, 93, 95, 96, 98, 99, 101, 103, 104, 106, 107, 109, 111, 112, 114, 116, 117, 119, 121, 123, 124, 126, 128, 130, 132, 133, 135, 137, 139, 141, 143, 145, 147, 149, 151, 154, 156, 158, 160, 163, 165, 168, 170, 173, 175, 178, 181, 184, 187, 190, 194, 197, 201, 205, 209, 214, 220, 226, 235, 255 };
int arcsin(int P, int A) {
    int angle = asin_lut[(divide(P << 11, A) >> 4) & 0x7F];
    return P > 0 ? angle : -angle;
}