#ifndef MARG_H
#define MARG_H

#include <system/fp/float32.h>

namespace Control {

class MARG {
private:
public:
    MARG();

    void update();
};

}

#endif