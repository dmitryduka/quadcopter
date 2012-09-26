#ifndef FLOAT32_H
#define FLOAT32_H

/* FreeBSD softfloat code as a class */
class float32 {
private:
    typedef int flag;
    typedef int uint8;
    typedef int int8;
    typedef int uint16;
    typedef int int16;
    typedef unsigned int uint32;
    typedef signed int int32;

    typedef unsigned char bits8;
    typedef signed char sbits8;
    typedef unsigned short int bits16;
    typedef signed short int sbits16;
    typedef unsigned int bits32;
    typedef signed int sbits32;

    typedef unsigned int float_type;
    mutable float_type data;

/* Numerous helper functions */
    static flag		extractSign(float32);
    static int16	extractExp(float32);
    static bits32	extractFrac(float32);

public:
    float32();
    float32(const float32&);

    float32 operator+(float32) const;
    float32 operator-(float32) const;
    float32 operator*(float32) const;
    float32 operator/(float32) const;
    float32& operator+=(float32);
    float32& operator-=(float32);
    float32& operator*=(float32);
    float32& operator/=(float32);

    bool operator==(float32) const;
    bool operator!=(float32) const;
    bool operator>(float32) const;
    bool operator<(float32) const;
    bool operator>=(float32) const;
    bool operator<=(float32) const;

    operator int() const;
};

#endif