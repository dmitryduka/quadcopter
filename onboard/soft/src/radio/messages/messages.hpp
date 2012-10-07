#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include <common/ct-utility.hpp>

namespace Radio {

namespace Messages {

/* Quad-centric definitions 

    Message type
    From - [1-126],
    To - [129-254] 

In order to add new message:
    * add it to the To/From enum
    * define message structure, 
	using private inheritance 
	from the Message type and
	specify it's type id in the
	template parameters.
    * in case it's To message, 
	specify it's handler
	in the handlers array.

*/

enum class To : char {
    MESSAGES_BEGIN = 0,
    Throttle = 0,
    Pitch,
    Yaw,
    Roll,
    ConsoleRequest = '>', // 60
    MESSAGES_COUNT = 5
};

enum class From : unsigned char {
    MESSAGES_BEGIN = 128,
    IMUData,
    PIDValues,
    ConsoleResponse = '<', // 62
    MESSAGES_COUNT = 3
};

/* Each message should contain it's type stored inside the first byte */
template<typename Type, Type TYPE> struct Message { const Type type = TYPE; };

/* ==============================================================
		Messages definitions 
   ============================================================== */
/* Telemetry messages */
struct IMUData : Message<From, From::IMUData> {
    short int Ax;
    short int Ay;
    short int Az;
    short int Gx;
    short int Gy;
    short int Gz;
    short int Cx;
    short int Cy;
    short int Cz;
    int P;
    int T;
};

struct PIDValues : Message<From, From::PIDValues> {
    short int P, I, D;
};

/* Control messages */
struct Throttle : Message<To, To::Throttle> { short int value; };
struct Pitch : Message<To, To::Pitch> { short int value; };
struct Yaw : Message<To, To::Yaw> { short int value; };
struct Roll : Message<To, To::Roll> { short int value; };

/* Console request message */
struct ConsoleRequest : Message<To, To::ConsoleRequest> { };

/* ==============================================================
		Messages handlers definitions 
   ============================================================== */
typedef void (*HandlerType)(char*);

void defaultHandler(char*);

void consoleHandler(char*);

struct EntryType {
    unsigned char size;
    HandlerType handler;
    
    void operator=(const EntryType& other) {
	size = other.size; handler = other.handler;
    }
};

const unsigned int MAX_MESSAGE_LENGTH = 32;

/* Setup Message Type -> Message size/Message handler mapping here */
#define DEFINE_MESSAGE_HANDLER(X, H) { sizeof(X), H }

const EntryType handlers[asIntegral<unsigned char, To>(To::MESSAGES_COUNT)] = {
    DEFINE_MESSAGE_HANDLER(Throttle,		defaultHandler),
    DEFINE_MESSAGE_HANDLER(Pitch,		defaultHandler),
    DEFINE_MESSAGE_HANDLER(Yaw,			defaultHandler),
    DEFINE_MESSAGE_HANDLER(Roll,		defaultHandler),
    DEFINE_MESSAGE_HANDLER(ConsoleRequest,	consoleHandler),
};

#undef DEFINE_MESSAGE_HANDLER

}

}

#endif
