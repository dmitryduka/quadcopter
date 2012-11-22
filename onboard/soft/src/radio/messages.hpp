#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <common>
#include <system>

#include "message_types.h"

namespace Radio {

namespace Messages {

/* Each message should contain it's type stored inside the first byte */
template<typename Type, Type TYPE> struct Message { 
    const Type type;
    Message() : type(TYPE) {}
    inline operator const char*() const { return reinterpret_cast<const char*>(this); }
};

/* Send message template function */
template<typename T>
void send(const T& x) {
    System::Bus::UART::write_waiting(x, sizeof(x));
}
/* ==============================================================
		Messages definitions 
   ============================================================== */

struct RegistrySet : public Message<To, To::REGISTRY_SET> {
    char offset;
    int value;
};

/* ==============================================================
		Messages handlers definitions 
   ============================================================== */
typedef void (*HandlerType)(char*);

void defaultHandler(char*);

void consoleHandler(char*);

struct EntryType {
    unsigned char size;
    HandlerType handler;
};

const unsigned int MAX_MESSAGE_LENGTH = 32;

/* Setup Message Type -> Message size/Message handler mapping here */
#define DEFINE_MESSAGE_HANDLER(X, H) { sizeof(X), H }

/* It's vital to place handlers in the same order as in the Radio::Messages::To */
const EntryType handlers[asIntegral<unsigned char, To>(To::MESSAGES_COUNT)] = {
    DEFINE_MESSAGE_HANDLER(To::REGISTRY_SET, defaultHandler)
};

#undef DEFINE_MESSAGE_HANDLER

}

}

#endif
