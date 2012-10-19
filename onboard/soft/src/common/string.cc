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

/*32 bit binary -> 64 bit hex */
ustring b32tohex(unsigned int x) {
	ustring ll = 0;
    const char* hex = "0123456789ABCDEF";
    for(int i = 7; i >= 0; --i) {
		ll |= ((ustring)hex[x >> 28]) << (8 * i);
		x <<= 4;
		}
    return ll;
}

ustring b32todec(int x) {
    const ustring tobig_p = 0x202020544F424947; // '   TOBIG'
    const ustring tobig_n = 0x20202D544F424947; // '  -TOBIG'
    /* check if return tobig_p/n or not */
    /* '-9999999' */
    if(Math::abs(x) <= 9999999) {
	int counter = 0;
	while(x != 0) {
	    ustring result = 0x2020202020202020; // '        '
	    int x10 = Math::divide(x, 10);
	    char digit = x - x10 * 10 + '0';
	    x = x10;
	    /* Space characters will not affect |=, because '0'-'9' use same bits */
	    result |= (ustring)digit << (8 * counter++);
	}
    } else {
	if(x < 0) return tobig_n;
	else return tobig_p;
    }
}
