#include <common>
#include "float32.h"

#define float32_default_nan 0xFFFFFFFF

float32::float32() {}

float32::float32(float a) { data = *reinterpret_cast<float_type*>(&a); }

float32::float32(int a) {
    flag zSign;

    if(a == 0) {
	data = 0;
	return;
    }
    if(a == (sbits32)0x80000000) {
	data = pack(1, 0x9E, 0);
	return;
    }
    zSign = a < 0;
    data = normalizeRoundAndPack(zSign, 0x9C, zSign ? - a : a);
}

float32::float32(unsigned int a) {
    if(a == 0) {
	data = 0;
	return;
    }
    data = normalizeRoundAndPack(0, 0x9C, a);
}

float32::float32(const float32& other) : data(other.data) {}

const float32 float32::operator+(const float32& other) const {
    float32 result;
    flag aSign, bSign;

    aSign = extractSign( data );
    bSign = extractSign( other.data );
    if (aSign == bSign) {
        result.data = addSigs( data, other.data, aSign );
    }
    else {
        result.data = subSigs( data, other.data, aSign );
    }
    return result;
}

const float32 float32::operator-(const float32& other) const {
    float32 result;
    flag aSign, bSign;
    float_type a = data;
    float_type b = other.data;

    aSign = extractSign( a );
    bSign = extractSign( b );
    if ( aSign == bSign ) {
        result.data = subSigs( a, b, aSign );
    }
    else {
        result.data = addSigs( a, b, aSign );
    }
    return result;
}

const float32 float32::operator*(const float32& other) const {
    float32 result;
    float_type a = data;
    float_type b = other.data;
    flag aSign, bSign, zSign;
    int16 aExp, bExp, zExp;
    bits32 aSig, bSig;

    aSig = extractFrac(a);
    aExp = extractExp(a);
    aSign = extractSign(a);
    bSig = extractFrac(b);
    bExp = extractExp(b);
    bSign = extractSign(b);
    zSign = aSign ^ bSign;
    if ( aExp == 0xFF ) {
        if ( ( bExp | bSig ) == 0 ) {
            result.data = float32_default_nan;
            return result;
        }
        result.data = pack( zSign, 0xFF, 0 );
        return result;
    }
    if ( bExp == 0xFF ) {
        if ( ( aExp | aSig ) == 0 ) {
            result.data = float32_default_nan;
        }
        result.data = pack(zSign, 0xFF, 0);
        return result;
    }
    if ( aExp == 0 ) {
        if ( aSig == 0 ) {
    	    result.data = pack( zSign, 0, 0 );
    	    return result;
    	}
        normalizeSubnormal(aSig, &aExp, &aSig);
    }
    if ( bExp == 0 ) {
        if ( bSig == 0 ) {
    	    result.data = pack(zSign, 0, 0);
    	    return result;
    	}
        normalizeSubnormal(bSig, &bExp, &bSig);
    }
    zExp = aExp + bExp - 0x7F;
    aSig = ( aSig | 0x00800000 ) << 7;
    bSig = ( bSig | 0x00800000 ) << 8;
    bits64 zSig = (bits64)aSig * (bits64)bSig;
    bits32 zSig1 = (bits32)zSig;
    bits32 zSig0 = (zSig >> 32);
    zSig0 |= ( zSig1 != 0 );
    if ( 0 <= (sbits32) ( zSig0 << 1 ) ) {
        zSig0 <<= 1;
        --zExp;
    }
    result.data = roundAndPack( zSign, zExp, zSig0 );
    return result;
}

const float32 float32::operator/(const float32& other) const {
    float32 result;
    float_type a = data;
    float_type b = other.data;
    flag aSign, bSign, zSign;
    int16 aExp, bExp, zExp;
    bits32 aSig, bSig, zSig;

    aSig = extractFrac( a );
    aExp = extractExp( a );
    aSign = extractSign( a );
    bSig = extractFrac( b );
    bExp = extractExp( b );
    bSign = extractSign( b );
    zSign = aSign ^ bSign;
    if ( aExp == 0xFF ) {
        if ( bExp == 0xFF ) {
    	    result.data = float32_default_nan;
            return result;
        }
        result.data = pack( zSign, 0xFF, 0 );
        return result;
    }
    if ( bExp == 0xFF ) {
	result.data = pack( zSign, 0, 0 );
        return result;
    }
    if ( bExp == 0 ) {
        if ( bSig == 0 ) {
            if ( ( aExp | aSig ) == 0 ) {
    		result.data = float32_default_nan;
	        return result;
            }
            result.data = pack( zSign, 0xFF, 0 );
            return result;
        }
        normalizeSubnormal( bSig, &bExp, &bSig );
    }
    if ( aExp == 0 ) {
        if ( aSig == 0 ) {
    	    result.data = pack( zSign, 0, 0 );
    	    return result;
    	}
        normalizeSubnormal( aSig, &aExp, &aSig );
    }
    zExp = aExp - bExp + 0x7D;
    aSig = ( aSig | 0x00800000 ) << 7;
    bSig = ( bSig | 0x00800000 ) << 8;
    if ( bSig <= ( aSig + aSig ) ) {
        aSig >>= 1;
        ++zExp;
    }
    zSig = estimateDiv64To32( aSig, 0, bSig );
    if ( ( zSig & 0x3F ) <= 2 ) {
	bits64 term = (bits64)bSig * (bits64)zSig;
	bits64 aSig64 = ((bits64)aSig) << 32;
	bits64 rem = aSig64 - term;
	bits32 rem1 = (bits32)rem;
	bits32 rem0 = (rem >> 32);
	bits64 bSig64 = bSig;
        //sub64( aSig, 0, term0, term1, &rem0, &rem1 );
        while ( (sbits32) rem0 < 0 ) {
            --zSig;
            rem += bSig64;
	    rem1 = (bits32)rem;
	    rem0 = (rem >> 32);
            //add64( rem0, rem1, 0, bSig, &rem0, &rem1 );
        }
        zSig |= ( rem1 != 0 );
    }
    result.data = roundAndPack( zSign, zExp, zSig );
    return result;
}

float32& float32::operator=(const float32& other) { data = other.data; return *this; }
float32& float32::operator=(float other) { return *this = float32(other); }

float32& float32::operator+=(const float32& other) { return *this = this->operator+(other); }
float32& float32::operator-=(const float32& other) { return *this = this->operator-(other); }
float32& float32::operator*=(const float32& other) { return *this = this->operator*(other); }
float32& float32::operator/=(const float32& other) { return *this = this->operator/(other); }

bool float32::operator==(const float32& other) const {
	float_type a = data;
	float_type b = other.data;
	if (((extractExp(a) == 0xFF) && extractFrac(a)) || 
		((extractExp(b) == 0xFF) && extractFrac(b))) return 0;
    return ( a == b ) || ( (bits32) ( ( a | b )<<1 ) == 0 );
}

bool float32::operator!=(const float32& other) const { return !this->operator==(other); }

bool float32::operator>(const float32& other) const { return !this->operator<=(other); }

bool float32::operator<(const float32& other) const {
    flag aSign, bSign;
    float_type a = data;
    float_type b = other.data;

    if (((extractExp(a) == 0xFF) && extractFrac(a)) || 
	((extractExp(b) == 0xFF) && extractFrac(b))) return 0;
    aSign = extractSign( a );
    bSign = extractSign( b );
    if ( aSign != bSign ) return aSign && ( (bits32) ( ( a | b )<<1 ) != 0 );
    return ( a != b ) && ( aSign ^ ( a < b ) );	
}

bool float32::operator>=(const float32& other) const { return !this->operator<(other); }

bool float32::operator<=(const float32& other) const {
	flag aSign, bSign;
	float_type a = data;
	float_type b = other.data;

	if (((extractExp(a) == 0xFF) && extractFrac(a)) || 
		((extractExp(b) == 0xFF) && extractFrac(b))) return 0;
    aSign = extractSign( a );
    bSign = extractSign( b );
    if ( aSign != bSign ) return aSign || ( (bits32) ( ( a | b )<<1 ) == 0 );
    return ( a == b ) || ( aSign ^ ( a < b ) );	
}

float32::operator float() const {
    return *reinterpret_cast<float*>(&data);
}

float32::operator int() const {
    flag aSign;
    int16 aExp, shiftCount;
    bits32 aSig, aSigExtra;
    int32 z;

    aSig = extractFrac(data);
    aExp = extractExp(data);
    aSign = extractSign(data);
    shiftCount = aExp - 0x96;
    if ( 0 <= shiftCount ) {
        if ( 0x9E <= aExp ) {
            if ( data!= 0xCF000000 ) {
                if ( ! aSign || ( ( aExp == 0xFF ) && aSig ) ) {
                    return 0x7FFFFFFF;
                }
            }
            return (sbits32) 0x80000000;
        }
        z = ( aSig | 0x00800000 ) << shiftCount;
        if ( aSign ) z = - z;
    }
    else {
        if ( aExp < 0x7E ) {
            aSigExtra = aExp | aSig;
            z = 0;
        }
        else {
            aSig |= 0x00800000;
            aSigExtra = aSig << ( shiftCount & 31 );
            z = aSig>>( - shiftCount );
        }
        if ( (sbits32) aSigExtra < 0 ) {
            ++z;
            if ( (bits32) ( aSigExtra<<1 ) == 0 ) z &= ~1;
        }
        if ( aSign ) z = - z;
    }
    return z;
}

float32::float_type float32::internals() const { return data; }

/*
-------------------------------------------------------------------------------
Returns the sign bit of the single-precision floating-point value `a'.
-------------------------------------------------------------------------------
*/
float32::flag float32::extractSign(float_type a) { return a >> 31; }

/*
-------------------------------------------------------------------------------
Returns the exponent bits of the single-precision floating-point value `a'.
-------------------------------------------------------------------------------
*/
float32::int16 float32::extractExp(float_type a) { return ( a >> 23 ) & 0xFF; }

/*
-------------------------------------------------------------------------------
Returns the fractioal bits of the single-precision floating-point value `a'.
-------------------------------------------------------------------------------
*/
float32::bits32 float32::extractFrac(float_type a) { return a & 0x007FFFFF; }

/*
-------------------------------------------------------------------------------
Returns the number of leading 0 bits before the most-significant 1 bit of
`a'.  If `a' is zero, 32 is returned.
-------------------------------------------------------------------------------
*/
float32::int8 float32::countLeadingZeros32( bits32 a )
{
    static const int8 countLeadingZerosHigh[] = {
        8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int8 shiftCount;

    shiftCount = 0;
    if ( a < 0x10000 ) {
        shiftCount += 16;
        a <<= 16;
    }
    if ( a < 0x1000000 ) {
        shiftCount += 8;
        a <<= 8;
    }
    shiftCount += countLeadingZerosHigh[ a>>24 ];
    return shiftCount;
}

/*
-------------------------------------------------------------------------------
Returns the result of subtracting the absolute values of the single-
precision floating-point values `a' and `b'.  If `zSign' is 1, the
difference is negated before being returned.  `zSign' is ignored if the
result is a NaN.  The subtraction is performed according to the IEC/IEEE
Standard for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/
float32::float_type float32::subSigs( float_type a, float_type b, flag zSign )
{
    int16 aExp, bExp, zExp;
    bits32 aSig, bSig, zSig;
    int16 expDiff;

    aSig = extractFrac( a );
    aExp = extractExp( a );
    bSig = extractFrac( b );
    bExp = extractExp( b );
    expDiff = aExp - bExp;
    aSig <<= 7;
    bSig <<= 7;
    if ( 0 < expDiff ) goto aExpBigger;
    if ( expDiff < 0 ) goto bExpBigger;
    if ( aExp == 0xFF ) {
        return float32_default_nan;
    }
    if ( aExp == 0 ) {
        aExp = 1;
        bExp = 1;
    }
    if ( bSig < aSig ) goto aBigger;
    if ( aSig < bSig ) goto bBigger;
    return pack( false, 0, 0 );
 bExpBigger:
    if ( bExp == 0xFF ) {
        return pack( zSign ^ 1, 0xFF, 0 );
    }
    if ( aExp == 0 ) {
        ++expDiff;
    }
    else {
        aSig |= 0x40000000;
    }
    shift32RightJamming( aSig, - expDiff, &aSig );
    bSig |= 0x40000000;
 bBigger:
    zSig = bSig - aSig;
    zExp = bExp;
    zSign ^= 1;
    goto normalizeRoundAndPack;
 aExpBigger:
    if ( aExp == 0xFF ) {
        return a;
    }
    if ( bExp == 0 ) {
        --expDiff;
    }
    else {
        bSig |= 0x40000000;
    }
    shift32RightJamming( bSig, expDiff, &bSig );
    aSig |= 0x40000000;
 aBigger:
    zSig = aSig - bSig;
    zExp = aExp;
 normalizeRoundAndPack:
    --zExp;
    return normalizeRoundAndPack( zSign, zExp, zSig );
}

/*
-------------------------------------------------------------------------------
Returns the result of adding the absolute values of the single-precision
floating-point values `a' and `b'.  If `zSign' is 1, the sum is negated
before being returned.  `zSign' is ignored if the result is a NaN.
The addition is performed according to the IEC/IEEE Standard for Binary
Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/
float32::float_type float32::addSigs( float_type a, float_type b, flag zSign )
{
    int16 aExp, bExp, zExp;
    bits32 aSig, bSig, zSig;
    int16 expDiff;

    aSig = extractFrac( a );
    aExp = extractExp( a );
    bSig = extractFrac( b );
    bExp = extractExp( b );
    expDiff = aExp - bExp;
    aSig <<= 6;
    bSig <<= 6;
    if ( 0 < expDiff ) {
        if ( aExp == 0xFF ) {
            return a;
        }
        if ( bExp == 0 ) {
            --expDiff;
        }
        else {
            bSig |= 0x20000000;
        }
        shift32RightJamming( bSig, expDiff, &bSig );
        zExp = aExp;
    }
    else if ( expDiff < 0 ) {
        if ( bExp == 0xFF ) {
            return pack( zSign, 0xFF, 0 );
        }
        if ( aExp == 0 ) {
            ++expDiff;
        }
        else {
            aSig |= 0x20000000;
        }
        shift32RightJamming( aSig, - expDiff, &aSig );
        zExp = bExp;
    }
    else {
        if ( aExp == 0xFF ) {
            return a;
        }
        if ( aExp == 0 ) return pack( zSign, 0, ( aSig + bSig )>>6 );
        zSig = 0x40000000 + aSig + bSig;
        zExp = aExp;
        goto roundAndPack;
    }
    aSig |= 0x20000000;
    zSig = ( aSig + bSig )<<1;
    --zExp;
    if ( (sbits32) zSig < 0 ) {
        zSig = aSig + bSig;
        ++zExp;
    }
 roundAndPack:
    return roundAndPack( zSign, zExp, zSig );
}

/*
-------------------------------------------------------------------------------
Shifts `a' right by the number of bits given in `count'.  If any nonzero
bits are shifted off, they are ``jammed'' into the least significant bit of
the result by setting the least significant bit to 1.  The value of `count'
can be arbitrarily large; in particular, if `count' is greater than 32, the
result will be either 0 or 1, depending on whether `a' is zero or nonzero.
The result is stored in the location pointed to by `zPtr'.
-------------------------------------------------------------------------------
*/
void float32::shift32RightJamming( bits32 a, int16 count, bits32 *zPtr )
{
    bits32 z;

    if(count == 0) z = a;
    else if(count < 32) z = (a >> count) | ((a << ((-count) & 31)) != 0);
    else z = a != 0;
    *zPtr = z;
}

/*
-------------------------------------------------------------------------------
Normalizes the subnormal single-precision floating-point value represented
by the denormalized significand `aSig'.  The normalized exponent and
significand are stored at the locations pointed to by `zExpPtr' and
`zSigPtr', respectively.
-------------------------------------------------------------------------------
*/
void float32::normalizeSubnormal(bits32 aSig, int16 *zExpPtr, bits32 *zSigPtr) {
    int8 shiftCount;

    shiftCount = countLeadingZeros32( aSig ) - 8;
    *zSigPtr = aSig << shiftCount;
    *zExpPtr = 1 - shiftCount;
}

/*
-------------------------------------------------------------------------------
Packs the sign `zSign', exponent `zExp', and significand `zSig' into a
single-precision floating-point value, returning the result.  After being
shifted into the proper positions, the three fields are simply added
together to form the result.  This means that any integer portion of `zSig'
will be added into the exponent.  Since a properly normalized significand
will have an integer portion equal to 1, the `zExp' input should be 1 less
than the desired result exponent whenever `zSig' is a complete, normalized
significand.
-------------------------------------------------------------------------------
*/
float32::float_type float32::pack(flag zSign, int16 zExp, bits32 zSig) {
	return (((bits32)zSign) << 31) + (((bits32)zExp) << 23) + zSig;
}

/*
-------------------------------------------------------------------------------
Takes an abstract floating-point value having sign `zSign', exponent `zExp',
and significand `zSig', and returns the proper single-precision floating-
point value corresponding to the abstract input.  Ordinarily, the abstract
value is simply rounded and packed into the single-precision format, with
the inexact exception raised if the abstract input cannot be represented
exactly.  However, if the abstract value is too large, the overflow and
inexact exceptions are raised and an infinity or maximal finite value is
returned.  If the abstract value is too small, the input value is rounded to
a subnormal number, and the underflow and inexact exceptions are raised if
the abstract input cannot be represented exactly as a subnormal single-
precision floating-point number.
    The input significand `zSig' has its binary point between bits 30
and 29, which is 7 bits to the left of the usual location.  This shifted
significand must be normalized or smaller.  If `zSig' is not normalized,
`zExp' must be 0; in that case, the result returned is a subnormal number,
and it must not require rounding.  In the usual case that `zSig' is
normalized, `zExp' must be 1 less than the ``true'' floating-point exponent.
The handling of underflow and overflow follows the IEC/IEEE Standard for
Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/
float32::float_type float32::roundAndPack( flag zSign, int16 zExp, bits32 zSig )
{
    int8 roundIncrement, roundBits;

    roundIncrement = 0x40;
    roundBits = zSig & 0x7F;
    if(0xFD <= (bits16)zExp) {
        if ((0xFD < zExp) || ((zExp == 0xFD) && ((sbits32)(zSig + roundIncrement) < 0)))
            return pack(zSign, 0xFF, 0) - (roundIncrement == 0);
        if ( zExp < 0 ) {
            shift32RightJamming(zSig, -zExp, &zSig);
            zExp = 0;
            roundBits = zSig & 0x7F;
        }
    }
    zSig = (zSig + roundIncrement ) >> 7;
    zSig &= ~ (((roundBits ^ 0x40) == 0) & 0x1);
    if(zSig == 0) zExp = 0;
    return pack(zSign, zExp, zSig);
}

/*
-------------------------------------------------------------------------------
Takes an abstract floating-point value having sign `zSign', exponent `zExp',
and significand `zSig', and returns the proper single-precision floating-
point value corresponding to the abstract input.  This routine is just like
`roundAndPack' except that `zSig' does not have to be normalized.
Bit 31 of `zSig' must be zero, and `zExp' must be 1 less than the ``true''
floating-point exponent.
-------------------------------------------------------------------------------
*/
float32::float_type float32::normalizeRoundAndPack(flag zSign, int16 zExp, bits32 zSig)
{
    int8 shiftCount;

    shiftCount = countLeadingZeros32(zSig) - 1;
    return roundAndPack(zSign, zExp - shiftCount, zSig << shiftCount);
}


/*
-------------------------------------------------------------------------------
Returns an approximation to the 32-bit integer quotient obtained by dividing
`b' into the 64-bit value formed by concatenating `a0' and `a1'.  The
divisor `b' must be at least 2^31.  If q is the exact quotient truncated
toward zero, the approximation returned lies between q and q + 2 inclusive.
If the exact quotient q is larger than 32 bits, the maximum positive 32-bit
unsigned integer is returned.
-------------------------------------------------------------------------------
*/
float32::bits32 float32::estimateDiv64To32( bits32 a0, bits32 a1, bits32 b )
{
    bits32 b0, b1;
    bits32 rem0, rem1;
    bits32 z;

    if ( b <= a0 ) return 0xFFFFFFFF;
    b0 = b>>16;
    z = ( b0<<16 <= a0 ) ? 0xFFFF0000 : Math::divide(a0, b0) << 16;
    bits64 term = (bits64)b * (bits64)z;
    bits64 a = ((bits64)a0 << 32) + a1;
    bits64 rem = a - term;
    rem1 = (bits32)rem;
    rem0 = (rem >> 32);
    while ( ( (sbits32) rem0 ) < 0 ) {
        z -= 0x10000;
        b1 = b << 16;
        rem += ((bits64)b0 << 32) + b1;
	rem1 = (bits32)rem;
	rem0 = (rem >> 32);
        //add64( rem0, rem1, b0, b1, &rem0, &rem1 );
    }
    rem0 = (rem0 << 16) | (rem1 >> 16);
    z |= (b0 << 16 <= rem0) ? 0xFFFF : Math::divide(rem0, b0);
    return z;
}

/* operators to support builtin float */
float32 operator+(float a, float32 b) {  return float32(a) + b; }
float32 operator-(float a, float32 b) {  return float32(a) - b; }
float32 operator*(float a, float32 b) {  return float32(a) * b; }
float32 operator/(float a, float32 b) {  return float32(a) / b; }

/* Math functions for the float32 type */
namespace f32 {


float32 log2(const float32& x) {
    union { float f; unsigned int i; } vx = { x };
    union { unsigned int i; float f; } mx = { (vx.i & 0x007FFFFF) | (0x7e << 23) };
    float32 y(vx.i);
    y *= float32(1.19209303e-7f);
    return y - float32(124.22544637f) - float32(1.498030302f) * mx.f - float32(1.72587999f) / (float32(0.3520887068f) + mx.f);
}

float32 pow2(const float32& p)
{
    union { unsigned int i; float32 f; } v = { (int)((p + 126.94269504f) * float32(1 << 23)) };
    return v.f;
}

float32 pow(const float32& x, const float32& p) { return pow2(p * log2(x)); }

/* Quake3 sqrt implementation */
float32 sqrt(const float32& x) {
#define SQRT_MAGIC_F 0x5f3759df 
    const float32 xhalf = 0.5f * x;
    union // get bits for floating value
    {
	float x;
	int i;
    } u;
    u.x = x;
    u.i = SQRT_MAGIC_F - (u.i >> 1);  // gives initial guess y0
    return float32(x) * u.x * (1.5f - xhalf * u.x * u.x);// Newton step, repeating increases accuracy 
}

float32 log10(const float32& x) {
    return log2(x) / float32(3.321928f);
}

/* http://en.wikipedia.org/wiki/Fast_inverse_square_root */
float32 rsqrt(const float32& y) {
    float32 x(y);
    const float32 threehalves(1.5f), half(0.5f);
    const float32 xhalf = half * x;
    int i = *(int *)&x;          // View x as an int.
    i = 0x5f375a86 - (i >> 1);   // Initial guess (slightly better).
    x = *(float *)&i;            // View i as float.
    x = x * (threehalves - xhalf * x * x);    // Newton step.
    //x = x * (threehalves - xhalf * x * x);    // Once more too increase accuracy
    return x;
}

float32 abs(const float32 x) {
    unsigned int *xx;
    xx = (unsigned int*)&x;
    *(xx) &= 2147483647u;
    return x;
}

/* http://devmaster.net/forums/topic/4648-fast-and-accurate-sinecosine/ */
/* Works only in [-PI; PI], precision ~ 0.001
 */
float32 sin(const float32& x) {
    const float PI = 3.14159265f;
    const float32 B(4.0f / PI);
    const float32 C(-4.0f / (PI * PI));
#define EXTRA_PRECISION
#ifdef EXTRA_PRECISION
//  const float Q = 0.775;
    const float32 P(0.225f);
    float32 y = B * x + C * x * abs(x);
    return P * (y * abs(y) - y) + y;   // Q * y + P * y * abs(y)
#else
    return B * x + C * x * abs(x);
#endif
}


float32 atan2(const float32& y, const float32& x)
{
   float32 angle;
   const float32 coeff_1(0.785398163f);
   const float32 coeff_2(2.35619449f);
   const float32 abs_y = abs(y) + 1e-10f;      // kludge to prevent 0/0 condition
   if (x >= 0.0f)
   {
      const float32 r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      const float32 r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }

    return  y < 0.0f ? (float32(0) - angle) : angle;     // negate if in quad III or IV
}

/* http://robots-everywhere.com/portfolio/math/fastatan2.htm */
float32 atan2_deg(const float32& x, const float32& y) {
    const float PI = 3.14159265f;
    const float32 c1(0.28088f); // empirical
    const float32 c2(180.0f / PI); // change const2 to 1.0 to get result in radians (I personally prefer working in degrees)
    const float32 c2n(-180.0f / PI); // change const2 to 1.0 to get result in radians (I personally prefer working in degrees)
    const float32 c3(90.0f); // this one here just means "quarter circle" so pi/4 or 90 degrees
    const float32 c3n(-90.0f); // this one here just means "quarter circle" so pi/4 or 90 degrees
    if(abs(x) > abs(y)) {
	return (x * y * c2n) / (x * x - c1 * y * y) + (x < 0.0f ? c3n : c3);
    } else {
	const float32 xy = x * y;
	return (xy < 0.0f ? c3 : c3n) + (x < 0.0f ? c3n : c3) + (c2 * xy) / (y * y + c1 * x * x);
    }
}

float32 asin(const float32& x) {
    const float32 scale_factor(0.391f);   //empirical
    float32 x5 = x * x;     //x^2
    x5 *= x5;           //x^4
    x5 *= x;            //x^5
    return x + scale_factor*x5;
}
}

