#ifndef MESSAGES_HPP
#define MESSAGES_HPP

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
    THROTTLE = 0,
    PITCH,
    YAW,
    ROLL,
    MESSAGES_COUNT
};

enum class From : unsigned char {
    MESSAGES_BEGIN = 128,
    IMU_DATA,
    PID_VALUES,
    MESSAGES_END
};

constexpr unsigned char asIntegral(To x) { return (unsigned char)x; }

/* Each message should contain it's type stored in 1 byte */
template<typename Type, Type TYPE> struct Message { const Type type = TYPE; };

/* ==============================================================
		Messages definitions 
   ============================================================== */
struct IMUData : Message<From, From::IMU_DATA> {
    short int Ax;
    short int Ay;
    short int Az;
    short int Gx;
    short int Gy;
    short int Gz;
    short int Cx;
    short int Cy;
    short int Cz;
};

struct PIDValues : Message<From, From::PID_VALUES> {
    short int X, Y;
};

struct Throttle : Message<To, To::THROTTLE> { short int value; };
struct Pitch : Message<To, To::PITCH> { short int value; };
struct Yaw : Message<To, To::YAW> { short int value; };
struct Roll : Message<To, To::ROLL> { short int value; };

/* ==============================================================
		Messages handlers definitions 
   ============================================================== */
typedef void (*HandlerType)();
const HandlerType defaultHandler = [](){};

/* Setup Message Type -> Message handler mapping here */
const HandlerType handlers[asIntegral(To::MESSAGES_COUNT) + 1] { 
    [To::THROTTLE] 	= defaultHandler,
    [To::PITCH] 	= defaultHandler,
    [To::YAW] 		= defaultHandler,
    [To::ROLL] 		= defaultHandler,
    /* This is here to generate compile-time error if not all messages were specified above */
    [To::MESSAGES_COUNT] = defaultHandler,
};

};

#endif
