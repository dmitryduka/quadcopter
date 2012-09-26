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
    static float_type   addSigs( float_type a, float_type b, flag zSign );
    static float_type   subSigs( float_type a, float_type b, flag zSign );
    static int8         countLeadingZeros32( bits32 a );
    static void         shift32RightJamming(bits32 a, int16 count, bits32 *zPtr);
    static flag         extractSign(float_type);
    static int16        extractExp(float_type);
    static bits32       extractFrac(float_type);
    static void         normalizeSubnormal(bits32 aSig, int16 *zExpPtr, bits32 *zSigPtr);
    static float_type   pack(flag zSign, int16 zExp, bits32 zSig);
    static float_type   roundAndPack(flag zSign, int16 zExp, bits32 zSig);
    static float_type   normalizeRoundAndPack(flag zSign, int16 zExp, bits32 zSig);
public:
    float32();
    float32(int);
    float32(const float32&);

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
};

#endif