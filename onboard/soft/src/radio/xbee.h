#ifndef XBEE_H
#define XBEE_H

#include <system>
#include <radio>

namespace Radio {
namespace Digital {

class XBeeReadIdleTask : public Tasks::IdleTask {
private:
    char message_buffer[Messages::MAX_MESSAGE_LENGTH + 1]; // reserve additional byte for trailing zero. console command parser uses strlen to determine the size of the command string
    unsigned int bytesSoFar;
    Messages::EntryType current_message;
public:
    XBeeReadIdleTask();
    void start();
};

}
}

#endif


