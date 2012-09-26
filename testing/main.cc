#include <iostream>

#include "float32.h"

template<typename T>
float check(T a, T b, T c, T d, T e) { 
    T result = a * b + c * d - e * d * d * d * d * d * d;
    return *reinterpret_cast<float*>(&result);
}

//=================== MAIN==============================//
int main() {
    float a = 1.02f, b = 2.02f, c = 3.02f, d = 5.02f, e = 0.01f;
    float32 a1(a), b1(b), c1(c), d1(d), e1(e);
    std::cout << "float : "<< check(a, b, c, d, e) << std::endl;
    std::cout << "float32 : "<< check(a1, b1, c1, d1, e1) << std::endl;
}

//=====================================================//
