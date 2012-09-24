//#include <stdio.h>
#include "sf.h"

/*
-------------------------------------------------------------------------------
Software IEC/IEEE floating-point underflow tininess-detection mode.
-------------------------------------------------------------------------------
*/
int8 float_detect_tininess;
enum {
    float_tininess_after_rounding  = 0,
    float_tininess_before_rounding = 1
};

/*
-------------------------------------------------------------------------------
Software IEC/IEEE floating-point rounding mode.
-------------------------------------------------------------------------------
*/
const int8 float_rounding_mode = 0;
enum {
    float_round_nearest_even = 0,
    float_round_to_zero      = 1,
    float_round_down         = 2,
    float_round_up           = 3
};

/*
-------------------------------------------------------------------------------
Software IEC/IEEE floating-point exception flags.
-------------------------------------------------------------------------------
*/
int8 float_exception_flags;
enum {
    float_flag_inexact   =  1,
    float_flag_underflow =  2,
    float_flag_overflow  =  4,
    float_flag_divbyzero =  8,
    float_flag_invalid   = 16
};

/*
-------------------------------------------------------------------------------
Raises the exceptions specified by `flags'.  Floating-point traps can be
defined here if desired.  It is currently not possible for such a trap to
substitute a result value.  If traps are not implemented, this routine
should be simply `float_exception_flags |= flags;'.
-------------------------------------------------------------------------------
*/
void float_raise( int8 flags )
{  float_exception_flags |= flags; }

/*
-------------------------------------------------------------------------------
Takes two single-precision floating-point values `a' and `b', one of which
is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
signaling NaN, the invalid exception is raised.
-------------------------------------------------------------------------------
*/
static float32 propagateFloat32NaN( float32 a, float32 b )
{ return 0; } //FIXME

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is a signaling
NaN; otherwise returns 0.
-------------------------------------------------------------------------------
*/
flag float32_is_signaling_nan( float32 a )
{  return ( ( ( a>>22 ) & 0x1FF ) == 0x1FE ) && ( a & 0x003FFFFF );  }

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
inline void shift32RightJamming( bits32 a, int16 count, bits32 *zPtr )
{   bits32 z;

    if ( count == 0 ) {
        z = a;	}
    else if ( count < 32 ) {
        z = ( a>>count ) | ( ( a<<( ( - count ) & 31 ) ) != 0 );	}
    else {
        z = ( a != 0 ); }
    *zPtr = z;	}

/*
-------------------------------------------------------------------------------
Multiplies `a' by `b' to obtain a 64-bit product.  The product is broken
into two 32-bit pieces which are stored at the locations pointed to by
`z0Ptr' and `z1Ptr'.
-------------------------------------------------------------------------------
*/
inline void mul32To64( bits32 a, bits32 b, bits32 *z0Ptr, bits32 *z1Ptr )
{
    bits16 aHigh, aLow, bHigh, bLow;
    bits32 z0, zMiddleA, zMiddleB, z1;

    aLow = a;
    aHigh = a>>16;
    bLow = b;
    bHigh = b>>16;
    z1 = ( (bits32) aLow ) * bLow;
    zMiddleA = ( (bits32) aLow ) * bHigh;
    zMiddleB = ( (bits32) aHigh ) * bLow;
    z0 = ( (bits32) aHigh ) * bHigh;
    zMiddleA += zMiddleB;
    z0 += ( ( (bits32) ( zMiddleA < zMiddleB ) )<<16 ) + ( zMiddleA>>16 );
    zMiddleA <<= 16;
    z1 += zMiddleA;
    z0 += ( z1 < zMiddleA );
    *z1Ptr = z1;
    *z0Ptr = z0;
}

/*
-------------------------------------------------------------------------------
Returns the number of leading 0 bits before the most-significant 1 bit of
`a'.  If `a' is zero, 32 is returned.
-------------------------------------------------------------------------------
*/
static int8 countLeadingZeros32( bits32 a )
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
    return shiftCount; }


/*
-------------------------------------------------------------------------------
Returns the fraction bits of the single-precision floating-point value `a'.
-------------------------------------------------------------------------------
*/
inline bits32 extractFloat32Frac( float32 a )
{  return a & 0x007FFFFF;	}

/*
-------------------------------------------------------------------------------
Returns the exponent bits of the single-precision floating-point value `a'.
-------------------------------------------------------------------------------
*/
inline int16 extractFloat32Exp( float32 a )
{  return ( a>>23 ) & 0xFF;  }

/*
-------------------------------------------------------------------------------
Returns the sign bit of the single-precision floating-point value `a'.
-------------------------------------------------------------------------------
*/
inline flag extractFloat32Sign( float32 a )
{  return a>>31; }

/*
-------------------------------------------------------------------------------
Normalizes the subnormal single-precision floating-point value represented
by the denormalized significand `aSig'.  The normalized exponent and
significand are stored at the locations pointed to by `zExpPtr' and
`zSigPtr', respectively.
-------------------------------------------------------------------------------
*/
static void
 normalizeFloat32Subnormal( bits32 aSig, int16 *zExpPtr, bits32 *zSigPtr )
{
    int8 shiftCount;

    shiftCount = countLeadingZeros32( aSig ) - 8;
    *zSigPtr = aSig<<shiftCount;
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
inline float32 packFloat32( flag zSign, int16 zExp, bits32 zSig )
{   return ( ( (bits32) zSign )<<31 ) + ( ( (bits32) zExp )<<23 ) + zSig;  }

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
float32 roundAndPackFloat32( flag zSign, int16 zExp, bits32 zSig ) //WAS_STATIC
{	//printf("roundAndPackFloat32(%d, %d, %x)\n", zSign, zExp, zSig);

	int8 roundBits = zSig & 0x7F;

    zSig = ( zSig + 0x40 )>>7;
    zSig &= ~ (( roundBits ^ 0x40 ) == 0);
    if ( zSig == 0 ) zExp = 0;
    return packFloat32( zSign, zExp, zSig );  } 

/*
-------------------------------------------------------------------------------
Takes an abstract floating-point value having sign `zSign', exponent `zExp',
and significand `zSig', and returns the proper single-precision floating-
point value corresponding to the abstract input.  This routine is just like
`roundAndPackFloat32' except that `zSig' does not have to be normalized.
Bit 31 of `zSig' must be zero, and `zExp' must be 1 less than the ``true''
floating-point exponent.
-------------------------------------------------------------------------------
*/ 
static float32
 normalizeRoundAndPackFloat32( flag zSign, int16 zExp, bits32 zSig )
{	//printf("normalizeRoundAndPackFloat32(%d, %d, %x)  ", zSign, zExp, zSig);

    int8 shiftCount;

    shiftCount = countLeadingZeros32( zSig ) - 1;

	//printf("Shift = %d\n", shiftCount);
    return roundAndPackFloat32( zSign, zExp - shiftCount, zSig<<shiftCount );

} 

/*
-------------------------------------------------------------------------------
Returns the result of converting the 32-bit two's complement integer `a' to
the single-precision floating-point format.  The conversion is performed
according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
float32 int32_to_float32( int32 a )
{
    flag zSign;

    if ( a == 0 ) return 0;
    if ( a == (sbits32) 0x80000000 ) return packFloat32( 1, 0x9E, 0 );
    zSign = ( a < 0 );
    return normalizeRoundAndPackFloat32( zSign, 0x9C, zSign ? - a : a );

} 

/*
-------------------------------------------------------------------------------
Returns the result of converting the single-precision floating-point value
`a' to the 32-bit two's complement integer format.  The conversion is
performed according to the IEC/IEEE Standard for Binary Floating-Point
Arithmetic---which means in particular that the conversion is rounded
according to the current rounding mode.  If `a' is a NaN, the largest
positive integer is returned.  Otherwise, if the conversion overflows, the
largest integer with the same sign as `a' is returned.
-------------------------------------------------------------------------------
*/ 
int32 float32_to_int32( float32 a )
{
    flag aSign;
    int16 aExp, shiftCount;
    bits32 aSig, aSigExtra;
    int32 z;
    int8 roundingMode;

    aSig = extractFloat32Frac( a );
    aExp = extractFloat32Exp( a );
    aSign = extractFloat32Sign( a );
    shiftCount = aExp - 0x96;
    if ( 0 <= shiftCount ) {
        if ( 0x9E <= aExp ) {
            if ( a != 0xCF000000 ) {
                float_raise( float_flag_invalid );
                if ( ! aSign || ( ( aExp == 0xFF ) && aSig ) ) {
                    return 0x7FFFFFFF;
                }
            }
            return (sbits32) 0x80000000;
        }
        z = ( aSig | 0x00800000 )<<shiftCount;
        if ( aSign ) z = - z;
    }
    else {
        if ( aExp < 0x7E ) {
            aSigExtra = aExp | aSig;
            z = 0;
        }
        else {
            aSig |= 0x00800000;
            aSigExtra = aSig<<( shiftCount & 31 );
            z = aSig>>( - shiftCount );
        }
        if ( aSigExtra ) float_exception_flags |= float_flag_inexact;
        roundingMode = float_rounding_mode;
        if ( roundingMode == float_round_nearest_even ) {
            if ( (sbits32) aSigExtra < 0 ) {
                ++z;
                if ( (bits32) ( aSigExtra<<1 ) == 0 ) z &= ~1;
            }
            if ( aSign ) z = - z;
        }
        else {
            aSigExtra = ( aSigExtra != 0 );
            if ( aSign ) {
                z += ( roundingMode == float_round_down ) & aSigExtra;
                z = - z;
            }
            else {
                z += ( roundingMode == float_round_up ) & aSigExtra;
            }
        }
    }
    return z;

}

/*
-------------------------------------------------------------------------------
Returns the result of converting the single-precision floating-point value
`a' to the 32-bit two's complement integer format.  The conversion is
performed according to the IEC/IEEE Standard for Binary Floating-Point
Arithmetic, except that the conversion is always rounded toward zero.
If `a' is a NaN, the largest positive integer is returned.  Otherwise, if
the conversion overflows, the largest integer with the same sign as `a' is
returned.
-------------------------------------------------------------------------------
*/ 
int32 float32_to_int32_round_to_zero( float32 a )
{
    flag aSign;
    int16 aExp, shiftCount;
    bits32 aSig;
    int32 z;

    aSig = extractFloat32Frac( a );
    aExp = extractFloat32Exp( a );
    aSign = extractFloat32Sign( a );
    shiftCount = aExp - 0x9E;
    if ( 0 <= shiftCount ) {
        if ( a != 0xCF000000 ) {
            float_raise( float_flag_invalid );
            if ( ! aSign || ( ( aExp == 0xFF ) && aSig ) ) return 0x7FFFFFFF;
        }
        return (sbits32) 0x80000000;
    }
    else if ( aExp <= 0x7E ) {
        if ( aExp | aSig ) float_exception_flags |= float_flag_inexact;
        return 0;
    }
    aSig = ( aSig | 0x00800000 )<<8;
    z = aSig>>( - shiftCount );
    if ( (bits32) ( aSig<<( shiftCount & 31 ) ) ) {
        float_exception_flags |= float_flag_inexact;
    }
    if ( aSign ) z = - z;
    return z;

}
/*
-------------------------------------------------------------------------------
Rounds the single-precision floating-point value `a' to an integer,
and returns the result as a single-precision floating-point value.  The
operation is performed according to the IEC/IEEE Standard for Binary
Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
float32 float32_round_to_int( float32 a )
{
    flag aSign;
    int16 aExp;
    bits32 lastBitMask, roundBitsMask;
    int8 roundingMode;
    float32 z;

    aExp = extractFloat32Exp( a );
    if ( 0x96 <= aExp ) {
        if ( ( aExp == 0xFF ) && extractFloat32Frac( a ) ) {
            return propagateFloat32NaN( a, a );
        }
        return a;
    }
    if ( aExp <= 0x7E ) {
        if ( (bits32) ( a<<1 ) == 0 ) return a;
        float_exception_flags |= float_flag_inexact;
        aSign = extractFloat32Sign( a );
        switch ( float_rounding_mode ) {
         case float_round_nearest_even:
            if ( ( aExp == 0x7E ) && extractFloat32Frac( a ) ) {
                return packFloat32( aSign, 0x7F, 0 );
            }
            break;
	 case float_round_to_zero:
	    break;
         case float_round_down:
            return aSign ? 0xBF800000 : 0;
         case float_round_up:
            return aSign ? 0x80000000 : 0x3F800000;
        }
        return packFloat32( aSign, 0, 0 );
    }
    lastBitMask = 1;
    lastBitMask <<= 0x96 - aExp;
    roundBitsMask = lastBitMask - 1;
    z = a;
    roundingMode = float_rounding_mode;
    if ( roundingMode == float_round_nearest_even ) {
        z += lastBitMask>>1;
        if ( ( z & roundBitsMask ) == 0 ) z &= ~ lastBitMask;
    }
    else if ( roundingMode != float_round_to_zero ) {
        if ( extractFloat32Sign( z ) ^ ( roundingMode == float_round_up ) ) {
            z += roundBitsMask;
        }
    }
    z &= ~ roundBitsMask;
    if ( z != a ) float_exception_flags |= float_flag_inexact;
    return z;

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
static float32 addFloat32Sigs( float32 a, float32 b, flag zSign )
{
    int16 aExp, bExp, zExp;
    bits32 aSig, bSig, zSig;
    int16 expDiff;

    aSig = extractFloat32Frac( a );
    aExp = extractFloat32Exp( a );
    bSig = extractFloat32Frac( b );
    bExp = extractFloat32Exp( b );
    expDiff = aExp - bExp;
    aSig <<= 6;
    bSig <<= 6;
    if ( 0 < expDiff ) {
        if ( aExp == 0xFF ) {
            if ( aSig ) return propagateFloat32NaN( a, b );
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
            if ( bSig ) return propagateFloat32NaN( a, b );
            return packFloat32( zSign, 0xFF, 0 );
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
            if ( aSig | bSig ) return propagateFloat32NaN( a, b );
            return a;
        }
        if ( aExp == 0 ) return packFloat32( zSign, 0, ( aSig + bSig )>>6 );
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
    return roundAndPackFloat32( zSign, zExp, zSig );

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
static float32 subFloat32Sigs( float32 a, float32 b, flag zSign )
{
    int16 aExp, bExp, zExp;
    bits32 aSig, bSig, zSig;
    int16 expDiff;

    aSig = extractFloat32Frac( a );
    aExp = extractFloat32Exp( a );
    bSig = extractFloat32Frac( b );
    bExp = extractFloat32Exp( b );
    expDiff = aExp - bExp;
    aSig <<= 7;
    bSig <<= 7;
    if ( 0 < expDiff ) goto aExpBigger;
    if ( expDiff < 0 ) goto bExpBigger;
    if ( aExp == 0xFF ) {
        if ( aSig | bSig ) return propagateFloat32NaN( a, b );
        float_raise( float_flag_invalid );
        return float32_default_nan;
    }
    if ( aExp == 0 ) {
        aExp = 1;
        bExp = 1;
    }
    if ( bSig < aSig ) goto aBigger;
    if ( aSig < bSig ) goto bBigger;
    return packFloat32( float_rounding_mode == float_round_down, 0, 0 );
 bExpBigger:
    if ( bExp == 0xFF ) {
        if ( bSig ) return propagateFloat32NaN( a, b );
        return packFloat32( zSign ^ 1, 0xFF, 0 );
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
        if ( aSig ) return propagateFloat32NaN( a, b );
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
    return normalizeRoundAndPackFloat32( zSign, zExp, zSig );

}

/*
-------------------------------------------------------------------------------
Returns the result of adding the single-precision floating-point values `a'
and `b'.  The operation is performed according to the IEC/IEEE Standard for
Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
float32 float32_add( float32 a, float32 b )
{
    flag aSign, bSign;

    aSign = extractFloat32Sign( a );
    bSign = extractFloat32Sign( b );
    if ( aSign == bSign ) {
        return addFloat32Sigs( a, b, aSign );
    }
    else {
        return subFloat32Sigs( a, b, aSign );
    }

}

/*
-------------------------------------------------------------------------------
Returns the result of subtracting the single-precision floating-point values
`a' and `b'.  The operation is performed according to the IEC/IEEE Standard
for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ /*
float32 float32_sub( float32 a, float32 b )
{
    flag aSign, bSign;

    aSign = extractFloat32Sign( a );
    bSign = extractFloat32Sign( b );
    if ( aSign == bSign ) {
        return subFloat32Sigs( a, b, aSign );
    }
    else {
        return addFloat32Sigs( a, b, aSign );
    }

}

/*
-------------------------------------------------------------------------------
Returns the result of multiplying the single-precision floating-point values
`a' and `b'.  The operation is performed according to the IEC/IEEE Standard
for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
float32 float32_mul( float32 a, float32 b )
{
    flag aSign, bSign, zSign;
    int16 aExp, bExp, zExp;
    bits32 aSig, bSig, zSig0, zSig1;

    aSig = extractFloat32Frac( a );
    aExp = extractFloat32Exp( a );
    aSign = extractFloat32Sign( a );
    bSig = extractFloat32Frac( b );
    bExp = extractFloat32Exp( b );
    bSign = extractFloat32Sign( b );
    zSign = aSign ^ bSign;
    if ( aExp == 0xFF ) {
        if ( aSig || ( ( bExp == 0xFF ) && bSig ) ) {
            return propagateFloat32NaN( a, b );
        }
        if ( ( bExp | bSig ) == 0 ) {
            float_raise( float_flag_invalid );
            return float32_default_nan;
        }
        return packFloat32( zSign, 0xFF, 0 );
    }
    if ( bExp == 0xFF ) {
        if ( bSig ) return propagateFloat32NaN( a, b );
        if ( ( aExp | aSig ) == 0 ) {
            float_raise( float_flag_invalid );
            return float32_default_nan;
        }
        return packFloat32( zSign, 0xFF, 0 );
    }
    if ( aExp == 0 ) {
        if ( aSig == 0 ) return packFloat32( zSign, 0, 0 );
        normalizeFloat32Subnormal( aSig, &aExp, &aSig );
    }
    if ( bExp == 0 ) {
        if ( bSig == 0 ) return packFloat32( zSign, 0, 0 );
        normalizeFloat32Subnormal( bSig, &bExp, &bSig );
    }
    zExp = aExp + bExp - 0x7F;
    aSig = ( aSig | 0x00800000 )<<7;
    bSig = ( bSig | 0x00800000 )<<8;
    mul32To64( aSig, bSig, &zSig0, &zSig1 );
    zSig0 |= ( zSig1 != 0 );
    if ( 0 <= (sbits32) ( zSig0<<1 ) ) {
        zSig0 <<= 1;
        --zExp;
    }
    return roundAndPackFloat32( zSign, zExp, zSig0 );

}



/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is equal to
the corresponding value `b', and 0 otherwise.  The comparison is performed
according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
flag float32_eq( float32 a, float32 b )
{

    if (    ( ( extractFloat32Exp( a ) == 0xFF ) && extractFloat32Frac( a ) )
         || ( ( extractFloat32Exp( b ) == 0xFF ) && extractFloat32Frac( b ) )
       ) {
        if ( float32_is_signaling_nan( a ) || float32_is_signaling_nan( b ) ) {
            float_raise( float_flag_invalid );
        }
        return 0;
    }
    return ( a == b ) || ( (bits32) ( ( a | b )<<1 ) == 0 );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is less than
or equal to the corresponding value `b', and 0 otherwise.  The comparison
is performed according to the IEC/IEEE Standard for Binary Floating-Point
Arithmetic.
-------------------------------------------------------------------------------
*/ 
flag float32_le( float32 a, float32 b )
{
    flag aSign, bSign;

    if (    ( ( extractFloat32Exp( a ) == 0xFF ) && extractFloat32Frac( a ) )
         || ( ( extractFloat32Exp( b ) == 0xFF ) && extractFloat32Frac( b ) )
       ) {
        float_raise( float_flag_invalid );
        return 0;
    }
    aSign = extractFloat32Sign( a );
    bSign = extractFloat32Sign( b );
    if ( aSign != bSign ) return aSign || ( (bits32) ( ( a | b )<<1 ) == 0 );
    return ( a == b ) || ( aSign ^ ( a < b ) );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is less than
the corresponding value `b', and 0 otherwise.  The comparison is performed
according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
flag float32_lt( float32 a, float32 b )
{
    flag aSign, bSign;

    if (    ( ( extractFloat32Exp( a ) == 0xFF ) && extractFloat32Frac( a ) )
         || ( ( extractFloat32Exp( b ) == 0xFF ) && extractFloat32Frac( b ) )
       ) {
        float_raise( float_flag_invalid );
        return 0;
    }
    aSign = extractFloat32Sign( a );
    bSign = extractFloat32Sign( b );
    if ( aSign != bSign ) return aSign && ( (bits32) ( ( a | b )<<1 ) != 0 );
    return ( a != b ) && ( aSign ^ ( a < b ) );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is equal to
the corresponding value `b', and 0 otherwise.  The invalid exception is
raised if either operand is a NaN.  Otherwise, the comparison is performed
according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
flag float32_eq_signaling( float32 a, float32 b )
{

    if (    ( ( extractFloat32Exp( a ) == 0xFF ) && extractFloat32Frac( a ) )
         || ( ( extractFloat32Exp( b ) == 0xFF ) && extractFloat32Frac( b ) )
       ) {
        float_raise( float_flag_invalid );
        return 0;
    }
    return ( a == b ) || ( (bits32) ( ( a | b )<<1 ) == 0 );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is less than or
equal to the corresponding value `b', and 0 otherwise.  Quiet NaNs do not
cause an exception.  Otherwise, the comparison is performed according to the
IEC/IEEE Standard for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
flag float32_le_quiet( float32 a, float32 b )
{
    flag aSign, bSign;
    int16 aExp, bExp;

    if (    ( ( extractFloat32Exp( a ) == 0xFF ) && extractFloat32Frac( a ) )
         || ( ( extractFloat32Exp( b ) == 0xFF ) && extractFloat32Frac( b ) )
       ) {
        if ( float32_is_signaling_nan( a ) || float32_is_signaling_nan( b ) ) {
            float_raise( float_flag_invalid );
        }
        return 0;
    }
    aSign = extractFloat32Sign( a );
    bSign = extractFloat32Sign( b );
    if ( aSign != bSign ) return aSign || ( (bits32) ( ( a | b )<<1 ) == 0 );
    return ( a == b ) || ( aSign ^ ( a < b ) );

}

/*
-------------------------------------------------------------------------------
Returns 1 if the single-precision floating-point value `a' is less than
the corresponding value `b', and 0 otherwise.  Quiet NaNs do not cause an
exception.  Otherwise, the comparison is performed according to the IEC/IEEE
Standard for Binary Floating-Point Arithmetic.
-------------------------------------------------------------------------------
*/ 
flag float32_lt_quiet( float32 a, float32 b )
{
    flag aSign, bSign;

    if (    ( ( extractFloat32Exp( a ) == 0xFF ) && extractFloat32Frac( a ) )
         || ( ( extractFloat32Exp( b ) == 0xFF ) && extractFloat32Frac( b ) )
       ) {
        if ( float32_is_signaling_nan( a ) || float32_is_signaling_nan( b ) ) {
            float_raise( float_flag_invalid );
        }
        return 0;
    }
    aSign = extractFloat32Sign( a );
    bSign = extractFloat32Sign( b );
    if ( aSign != bSign ) return aSign && ( (bits32) ( ( a | b )<<1 ) != 0 );
    return ( a != b ) && ( aSign ^ ( a < b ) );

}
