#ifndef MOVING_AVERAGE_HPP
#define MOVING_AVERAGE_HPP

/* N is window width, should be power of 2 */
template<int N>
class moving_average
{
public:
    moving_average() : num_samples_(0), total_(0) { 
	for(int i = 0; i < N; ++i) samples_[i] = 0;
    }

    moving_average& operator()(int sample) {
        int& oldest = samples_[num_samples_++];
        if(num_samples_ == N) num_samples_ = 0;
        total_ += sample - oldest;
        oldest = sample;
        return *this;
    }

    operator int() const { return total_ >> log_<N>::value; }

private:
    int samples_[N];
    int num_samples_;
    int total_;
};

#endif