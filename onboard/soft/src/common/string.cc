#include "string.h"

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

ustring lb32tohex(unsigned int x)
{	ustring l = 0L;
	for(int i = 0; i < 8; ++i)
	{	char c = (x >> 28) & 0xF;
		if(c < 10) c += '0';
		else	   c += 'A' - 10;
		l |= c;
		if(i == 7) break;
		l <<= 8;	}

	return l; }
