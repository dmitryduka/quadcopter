#ifndef MEDIAN_FILTER_HPP
#define MEDIAN_FILTER_HPP

#include <common>

namespace Math {
/* N is window width, should be power of 2 */
template<typename T, int N>
class median_filter
{
public:
    median_filter() : num_samples_(0), total_(0) {
        for (int i = 0; i < N; ++i) samples_[i] = 0;
    }

    median_filter& operator()(T sample) {
        T& oldest = samples_[num_samples_++];
        if (num_samples_ == N) num_samples_ = 0;
        total_ += sample - oldest;
        oldest = sample;
        return *this;
    }

    operator T() const { 
	T temp[N];
        for (int i = 0; i < N; ++i) temp[i] = samples_[i];
	qsort(temp, 0, N);
	return temp[N >> 1]; 
    }

private:
    T samples_[N];
    int num_samples_;
    int total_;
};

}

#endif