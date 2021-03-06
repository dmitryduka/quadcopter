#ifndef CT_UTILITY_HPP
#define CT_UTILITY_HPP

#include <system/devices.hpp>

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

/* Time conversions */
typedef unsigned int Ticks;
constexpr Ticks operator"" _hz(unsigned long long x) {   return CPU_FREQUENCY_HZ / x;}
constexpr Ticks operator"" _ns(unsigned long long x) {   return x * CPU_FREQUENCY_HZ / 1000000000;}
constexpr Ticks operator"" _us(unsigned long long x) {   return x * CPU_FREQUENCY_HZ / 1000000;}
constexpr Ticks operator"" _ms(unsigned long long x) {   return x * CPU_FREQUENCY_HZ / 1000;}
constexpr Ticks operator"" _s(unsigned long long x) {    return x * CPU_FREQUENCY_HZ;}

/* */
template<typename To, typename From>
constexpr To asIntegral(From x) { return static_cast<To>(x); }


#endif
