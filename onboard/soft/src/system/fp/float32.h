#ifndef FLOAT32_H
#define FLOAT32_H

/* FreeBSD softfloat code as a class */
class float32 {
private:
    typedef bool flag;
    typedef char uint8;
    typedef char int8;
    typedef int uint16;
    typedef int int16;
    typedef unsigned int uint32;
    typedef signed int int32;
    typedef long long int64;
    typedef unsigned long long uint64;

    typedef unsigned char bits8;
    typedef signed char sbits8;
    typedef unsigned short int bits16;
    typedef signed short int sbits16;
    typedef unsigned int bits32;
    typedef signed int sbits32;
    typedef unsigned long long bits64;
    typedef long long sbits64;

    typedef unsigned int float_type;
    mutable float_type data;

/* Numerous helper functions */
    static bits32	estimateDiv64To32(bits32, bits32, bits32);
    static float_type   addSigs(float_type, float_type, flag);
    static float_type   subSigs(float_type, float_type, flag);
    static int8         countLeadingZeros32(bits32);
    static void         shift32RightJamming(bits32, int16, bits32*);
    static flag         extractSign(float_type);
    static int16        extractExp(float_type);
    static bits32       extractFrac(float_type);
    static void         normalizeSubnormal(bits32, int16*, bits32*);
    static float_type   pack(flag, int16, bits32);
    static float_type   roundAndPack(flag, int16 zExp, bits32);
    static float_type   normalizeRoundAndPack(flag, int16, bits32);
public:
    float32();
    explicit float32(float);
    explicit float32(int);
    float32(const float32&);

    float32& operator=(float);
    float32& operator=(float32);

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
    operator float() const;
};

#endif