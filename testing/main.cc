#include <iostream>
#include <math.h>
#include "float32.h"

template<typename Func1, typename Func2>
void test(Func1 func1, Func2 func2, float x1, float x2, float inc) {
    float max_diff = 0.0f;
    for(float i = x1; i < x2; i += inc) {
	float32 t(i);
	float diff = (float)func2(i) - (float)func1(t);
	std::cout << (float)i  << ": " << (float)func1(t) <<  "       " << func2(i) << ", diff = " << diff << std::endl;
	if(fabs(diff) > max_diff) max_diff = fabs(diff);
    }
    std::cout << "Max diff . = " << max_diff << std::endl;
}

template<typename Func1, typename Func2>
void test2(Func1 func1, Func2 func2, float x1, float x2, float y1, float y2, float inc) {
    float max_diff = 0.0f;
    for(float j = y1; j < y2; j += inc) {
	for(float i = x1; i < x2; i += inc) {
	    float32 t(i), k(j);
	    float diff = (float)func2(i,j) - (float)func1(t,k);
	    std::cout << i << "/" << j << "     " << (float)func1(t,k) <<  "       " << func2(i,k) << ", diff = " << diff << std::endl;
	    if(fabs(diff) > max_diff) max_diff = fabs(diff);
	}
    }
    std::cout << "Max diff . = " << max_diff << std::endl;
}

float rsqrt(float x) { return 1.0f / sqrt(x); }
float atan2_deg(float x, float y) { return atan2(x, y) * 180.0f / M_PI; }
float pow2(float y) { return pow(2.0f, y); }

int main() {
    test(f32::log10, log10, 0.1f, 10.0f, 0.1f);
}

