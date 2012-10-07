#include "string.h"

unsigned int strlen(const char* str) {
    unsigned int size = 0;
    while(*str) {
	size++;
	str++;
    }
    return size;
}

const char* b32tohex(unsigned int x) {
    static char result[9] = {};
    const char* hex_digits = "0123456789ABCDEF";
    for(int i = 0; i < sizeof(int) << 2; ++i) {
	result[i] = hex_digits[x & 0xF];
	x >>= 4;
    }
    result[8] = '\0';
    return result;
}
