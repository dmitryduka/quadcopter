#include "string.h"
#include "maths.hpp"

unsigned int strlen(const char* str) {
    unsigned int size = 0;
    while(*str) {
	size++;
	str++;
    }
    return size;
}

/* 32 bit binary -> 64 bit hex */
ustring b32tohex(unsigned int x) {
    ustring ll = 0;
    const char* hex = "0123456789ABCDEF";
    for(int i = 7; i >= 0; --i) {
	ll |= ((ustring)hex[x >> 28]) << (8 * i);
	x <<= 4;
    }
     return ll;
}

ustring b32todec(int y) {
    const ustring tobig_p = 0x20202B544F424947; // '  +TOBIG'
    const ustring tobig_n = 0x20202D544F424947; // '  -TOBIG'
    /* check if return tobig_p/n or not */
    /* '-9999999' */
    int x = Math::abs(y);
    if(x <= 9999999) {
	int counter = 0;
	ustring result = 0x2020202020202020; // '        '
	if(x == 0) return result |= (ustring)'0';
	while(x != 0) {
	    int x10 = Math::divide(x, 10);
	    char digit = x - x10 * 10 + '0';
	    x = x10;
	    /* Space characters will not affect |=, because '0'-'9' use the same bits */
	    result |= (ustring)digit << (8 * counter++);
	}
	result |= (ustring)(y < 0 ? '-' : '+') << (8 * counter);
	return result;
    } else {
	if(y < 0) return tobig_n;
	else return tobig_p;
    }
}

ustring b32todec(unsigned int y) {
    const ustring tobig = 0x202020544F424947; // '  +TOBIG'
    /* check if return tobig_p/n or not */
    /* '-9999999' */
    int x = Math::abs(y);
    if(x <= 99999999) {
	int counter = 0;
	ustring result = 0x2020202020202020; // '        '
	if(x == 0) return result |= (ustring)'0';
	while(x != 0) {
	    int x10 = Math::divide(x, 10);
	    char digit = x - x10 * 10 + '0';
	    x = x10;
	    /* Space characters will not affect |=, because '0'-'9' use the same bits */
	    result |= (ustring)digit << (8 * counter++);
	}
	return result;
    } else return tobig;
}

ustring f32todec(const float32& y) {
    const float32 D(1000.0f);
    const float32 x = f32::abs(y);
    if(x > D) return 0x202020544F424947; // '  +TOBIG'
    int bd = x;
    int ad = x * D - float32(bd) * D;
    if(ad < 0) {
	bd -= 1;
	ad = x * D - float32(bd) * D;
    }
    int counter = 0;
    ustring result = 0x202020202E202020; // '        '
    if((bd == 0) & (ad == 0)) return result |= (ustring)'0';
    for(int i = 0; i < 3; ++i) {
	int ad10 = Math::divide(ad, 10);
	char digit = ad - ad10 * 10 + '0';
	if(ad - ad10 * 10 < 0) digit = '0';
	ad = ad10;
	/* Space characters will not affect |=, because '0'-'9' use the same bits */
	result |= (ustring)digit << (8 * counter++);
    }
    counter++;
    while(bd != 0) {
	int bd10 = Math::divide(bd, 10);
	char digit = bd - bd10 * 10 + '0';
	bd = bd10;
	/* Space characters will not affect |=, because '0'-'9' use the same bits */
	result |= (ustring)digit << (8 * counter++);
    }
    result |= (ustring)(y < float32(0) ? '-' : '+') << (8 * counter);
    return result;
}