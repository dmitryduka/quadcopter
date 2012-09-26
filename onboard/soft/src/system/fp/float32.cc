#include "float32.h"

float32::float32() : data(0) {}
float32::float32(const float32& other) : data(other.data) {
}

float32 float32::operator+(float32) const {
    float32 result;
    return result;
}

float32 float32::operator-(float32) const {
    float32 result;
    return result;
}

float32 float32::operator*(float32) const {
    float32 result;
    return result;
}

float32 float32::operator/(float32) const {
    float32 result;
    return result;
}

float32& float32::operator+=(float32) { return *this; }

float32& float32::operator-=(float32) { return *this; }

float32& float32::operator*=(float32) { return *this; }

float32& float32::operator/=(float32) { return *this; }

bool float32::operator==(float32 other) const {return true;}
bool float32::operator!=(float32 other) const {return !this->operator==(other);}

bool float32::operator>(float32 other) const { return !this->operator<=(other); }

bool float32::operator<(float32 other) const {}

bool float32::operator>=(float32 other) const {return !this->operator<(other);}

bool float32::operator<=(float32 other) const {}

float32::operator int() const {}

float32::flag float32::extractSign(float32 a) { return a.data >> 31; }
float32::int16 float32::extractExp(float32 a) { return ( a.data >> 23 ) & 0xFF; }
float32::bits32 float32::extractFrac(float32 a) { return a.data & 0x007FFFFF; }