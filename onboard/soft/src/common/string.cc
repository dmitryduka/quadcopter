#include "string.h"

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
    union {
	ustring ll;
	unsigned char q[8];
    };
    const char* hex = "0123456789ABCDEF";
    for(int i = 0; i  < 8; ++i) {
	q[i] = hex[x >> 28];
	x <<= 4;
    }
    return ll;
}
