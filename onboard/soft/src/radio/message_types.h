#ifndef MESSAGE_TYPES
#define MESSAGE_TYPES

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

enum class To : int {
    MESSAGES_BEGIN = 0,
    REGISTRY_SET,
    BINARY_MESSAGES_COUNT,
    CONSOLE_REQUEST = '>', // 60
    MESSAGES_COUNT = BINARY_MESSAGES_COUNT
};

enum class From : unsigned int {
    MESSAGES_BEGIN = 128,
    REGISTRY_SINGLE_GET,
    REGISTRY_MULTIPLE_GET,
    BINARY_MESSAGES_COUNT,
    CONSOLE_RESPONSE = '<',
    MESSAGES_COUNT = BINARY_MESSAGES_COUNT
};

}
}

#endif
