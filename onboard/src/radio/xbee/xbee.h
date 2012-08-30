#ifndef XBEE_H
#define XBEE_H

#include <system/tasking/tasks.h>
#include <radio/messages/messages.hpp>

namespace Radio {
namespace Digital {

class XBeeReadIdleTask : public System::Tasking::IdleTask {
private:
    char message_buffer[Messages::MAX_MESSAGE_LENGTH];
    unsigned int bytesSoFar;
    Messages::EntryType handler;
public:
    XBeeReadIdleTask();
    void start();
};

}
}

#endif


