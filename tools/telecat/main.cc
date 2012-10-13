#include <iostream>
#include <cstdio>
#include <cstring>
#include <radio/messages/messages.hpp>

namespace Radio {
namespace Messages {

void consoleHandler(char* str) {}
void defaultHandler(char* str) {}

void quatDataHandler(char* str) {
    QuatData* data = reinterpret_cast<QuatData*>(str);
    float q1 = *reinterpret_cast<float*>(&data->q1);
    float q2 = *reinterpret_cast<float*>(&data->q2);
    float q3 = *reinterpret_cast<float*>(&data->q3);
    float q4 = *reinterpret_cast<float*>(&data->q4);
    std::cout << q1 << "\t" << q2 << "\t" << q3 << "\t" << q4 << std::endl;
}

void imuDataHandler(char* str) {
    IMUData* data = reinterpret_cast<IMUData*>(str);
    std::cout << data->Ax << "\t" << data->Ay << "\t" << data->Az << "\t" <<  
		data->Gx << "\t" << data->Gy << "\t" << data->Gz << "\t" <<  
		data->Cx << "\t" << data->Cy << "\t" << data->Cz << "\t" <<  
		data->P << "\t" << data->T << "\t" << std::endl;
}

void pidDataHandler(char* str) {
    PIDValues* data = reinterpret_cast<PIDValues*>(str);
    std::cout << data->P << "\t" << data->I << "\t" << data->D << "\t" << std::endl;
}


Radio::Messages::EntryType fromHandlers[static_cast<int>(Radio::Messages::From::MESSAGES_COUNT)] = {
    {sizeof(QuatData), quatDataHandler},
    {sizeof(IMUData), imuDataHandler},
    {sizeof(PIDValues), pidDataHandler}
};

}
}

int main() {
/*
    float t[4] = {0.99f, 0.33f, 0.123f, 0.45f};
    Radio::Messages::QuatData x;
    memcpy(&x.q1, t, sizeof(t));
    char *str = reinterpret_cast<char*>(&x);
    for(int i = 0; i < sizeof(Radio::Messages::QuatData); ++i)
	std::putchar(str[i]);
    return 0;
*/
    char message_buffer[Radio::Messages::MAX_MESSAGE_LENGTH + 1];
    size_t bytesSoFar = 0, messageCounter = 0;
    Radio::Messages::EntryType current_message = {0, NULL};
    while(1) {
	if(current_message.handler == NULL) {
	    char ch[4];
	    ch[0] = std::getchar();
	    ch[1] = std::getchar();
	    ch[2] = std::getchar();
	    ch[3] = std::getchar();
	    int type = *reinterpret_cast<int*>(ch);
	    std::cout << "type =  " << type << std::endl;
	    /* awaiting for the first character that could be interpreted as a message header/type */
	    if(type >= static_cast<unsigned int>(Radio::Messages::From::MESSAGES_BEGIN) && 
		type < static_cast<unsigned int>(Radio::Messages::From::MESSAGES_END)) {
		current_message = Radio::Messages::fromHandlers[type - static_cast<unsigned int>(Radio::Messages::From::MESSAGES_BEGIN)];
		message_buffer[bytesSoFar++] = ch[0];
		message_buffer[bytesSoFar++] = ch[1];
		message_buffer[bytesSoFar++] = ch[2];
		message_buffer[bytesSoFar++] = ch[3];
	    } else std::cout << "Invalid message type." << std::endl;
	} else {
	    char ch = std::getchar();
	    message_buffer[bytesSoFar++] = ch;
	    if(bytesSoFar == sizeof(message_buffer)) {
		current_message = {0, NULL};
		bytesSoFar = 0;
		std::cout << "Buffer overflow - reset." << std::endl;
	    }
	    if(bytesSoFar == current_message.size) {
		std::cout << "Message " << messageCounter << ": ";
		current_message.handler(message_buffer);
		messageCounter++;
		bytesSoFar = 0;
		current_message = {0, NULL};
	    }
	}
    }
}
