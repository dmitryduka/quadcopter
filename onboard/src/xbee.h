#ifndef XBEE_H
#define XBEE_H

#include "tasks.h"
#include "messages.hpp"

class XBeeReadIdleTask : public IdleTask {
private:
    char message_buffer[Messages::MAX_MESSAGE_LENGTH];
    unsigned int bytesSoFar;
    Messages::EntryType handler;
public:
    XBeeReadIdleTask();
    void start();
};

#endif


