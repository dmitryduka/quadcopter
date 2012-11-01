#ifndef STRING_H
#define STRING_H

#include <system/types.h>
#include <system/float32.h>

unsigned int strlen(const char*);
ustring b32tohex(unsigned int);
ustring b32todec(int);
ustring b32todec(unsigned int);
ustring f32todec(const float32&);

#endif
