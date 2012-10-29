#ifndef PID_H
#define PID_H

#include <system>
#include <common>

namespace Control {

#define SR System::Registry

struct Internal {
    int old;
    inline int operator()(int a) {
	int r = a - old;
	old = a;
	return r;
    }
};

template<SR::IntegerValue V>
struct External {
    inline int operator()(int a) { return SR::value(V); }
};

/* We want to use this class for two types of PIDs:
    * D term operates on the value itself 
	(i.e. uses diff between old/new values)
    * D term operates on some external value 
	(like pitch/roll PID uses gyro directly to damp PI)

    Hence the template parameter defining which type do we want */
template<typename DStrategy = Internal>
class PID {
public:
private:
    int& P;
    int& I;
    int& D;
    int& I_MAX;
    int& value;
    int& desired;
    int old_value;
    int output;
    int Iterm;

    DStrategy diff;
public:
    PID(SR::IntegerValue _P, 
	SR::IntegerValue _I, 
	SR::IntegerValue _D, 
	SR::IntegerValue _I_MAX, 
	SR::IntegerValue val, 
	SR::IntegerValue des) : P(SR::value(_P)),
				I(SR::value(_I)),
				D(SR::value(_D)),
				I_MAX(SR::value(_I_MAX)),
				value(SR::value(val)),
				desired(SR::value(des)) {}

    static int scale(int K, int V) { return (K * V) >> 8; }

    PID& operator()() {
	int err = value - desired;
	Iterm += err;

	if ((value ^ old_value) >> 31) Iterm = 0;
        else if (Math::abs(Iterm) < (unsigned int)I_MAX) Iterm += err;

	/* TODO: Check that shifts in the scale function do not
	    produce corrupt results for negative numbers */
	output = scale(P, err) +
		scale(I, Iterm) +
		scale(D, diff(value));

	old_value = value;
	return *this;
    }

    operator int() const { return output; }
};

}

#endif