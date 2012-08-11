#ifndef CT_UTILITY_HPP
#define CT_UTILITY_HPP

#include "devices.hpp"

#define forever while(1)

typedef unsigned int size_t;

/* Compile time log (to compute shift) */
template <size_t N, size_t base=2> struct log_ {
    enum { value = 1 + log_<N / base, base>::value };
};
template <size_t base> struct log_<1, base> {
    enum { value = 0 };
};
template <size_t base> struct log_<0, base> {
    enum { value = 0 };
};

/* Q should be power of 2 */
template<int P, int Q> inline int scale(int x) {
    return (x * P) >> log_<Q>::value;
}

#endif