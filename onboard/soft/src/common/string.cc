#include "string.h"

unsigned int strlen(const char* str) {
    unsigned int size = 0;
    while(*str++) size++;
    return size;
}

unsigned int b32tohex(unsigned int x) {
    unsigned int result = 0;
    const char* hex_digits = "0123456789ABCDEF";
    result  = hex_digits[(x & 0xF000) >> 24] << 24;
    result |= hex_digits[(x & 0x0F00) >> 16] << 16;
    result |= hex_digits[(x & 0x00F0) >> 8] << 8;
    result |= hex_digits[(x & 0x000F)];
    return result;
}
