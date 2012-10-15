#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <system>

namespace Radio {
namespace Digital {

class TelemetryTask : public System::Tasking::ContinuousTask {
public:
    void start();
};

}
}

#endif