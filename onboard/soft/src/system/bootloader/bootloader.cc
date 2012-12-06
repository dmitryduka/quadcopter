#include <system>

/* After bootloader starts, it is ready to accept several types
    of messages: ping, get the CODE_STARTS address, 
    set address, memory write/read, exit to the main code

    Each message starts with the message type - 1 byte.

    * Ping message
    [TO BOOTLOADER]     MsgType ('?')
    [FROM BOOTLOADER]   MsgType ('!')

    * get the CODE_STARTS address message
    [TO BOOTLOADER]     MsgType ('^')
    [FROM BOOTLOADER]   MsgType ('^') | (Address, 4 bytes) | (CRC16 of all message after MsgType, 2 bytes)

    * set the start address for write operations
    [TO BOOTLOADER]     MsgType ('%') | (Address, 4 bytes) | (CRC16 of all message after MsgType, 2 bytes)
    [FROM BOOTLOADER]   MsgType ('%') | '!'
    or, in case, any errors (CRC is not right)
    [FROM BOOTLOADER]   MsgType ('%') | '?'

    * Memory write/read message 
    [TO BOOTLOADER]     MsgType (0) | (SizeInBytes, 2 bytes) | (Data, SizeInBytes bytes) | (CRC16 of all message after MsgType, 2 bytes)
    [FROM BOOTLOADER]   MsgType (0) | (SizeInBytesWritten, 2 bytes) | (CRC16 of all message after MsgType, 2 bytes)
        In case any errors, SizeInBytesWritten will be less than SizeInBytes (requested to write), 
        so the operation should be repeated.
    [TO BOOTLOADER]     MsgType (1) | (SizeInBytes, 2 bytes)  | (CRC16 of data, 2 bytes)
    [FROM BOOTLOADER]   MsgType (1) | (Data) | (CRC16 of data, 2 bytes)

    * Exit to main code message *
    [TO BOOTLOADER]     MsgType ('@')
    [FROM BOOTLOADER]   MsgType ('!')

    * Illegal message (unknown type), immediately after the first illegal byte received after
        leaving awaiting-for-message state
    [FROM BOOTLOADER]  '?'
*/

enum BootloaderMessageType {
    WRITE,
    READ,
    EXIT = '@',
    PING = '?',
    CODESTARTS = '^',
    SETADDRESS = '%'
};

constexpr static unsigned int DATA_LENGTH_SIZE = 2;
constexpr static unsigned int MAX_DATA_LENGTH = 32;
constexpr static char OK_RESPONSE = '!';
constexpr static char DONT_UNDERSTAND = '?';

/* We need to get this function inlined for bootloader to be fully independend of
    any other code, which might get overriden during "flash" process */

#define BOOTLOADER __attribute__((section(".bootloader")))

unsigned short crc16(const unsigned char *pcBlock, unsigned short len) BOOTLOADER;
#include <crc16.hpp>
void uart_write_waiting(char x) BOOTLOADER;
char uart_read_waiting() BOOTLOADER;
void uart_write_write_message_response(unsigned short int size) BOOTLOADER;
void * read_address() BOOTLOADER;
unsigned short int read_2b() BOOTLOADER;

extern unsigned char* BOOTLOADER_STARTS;
extern unsigned char* BOOTLOADER_ENDS;
extern unsigned char CODE_STARTS;

void uart_write_waiting(char x) 
{
    while(System::Bus::UART::TX_BUFFER_LENGTH - *DEV_UART_TX == 0) { }
    *DEV_UART_TX = x;
}

char uart_read_waiting() 
{
    while((*DEV_UART_RX >> 16) == 0) { }
	char c = *DEV_UART_RX & 0xFF;
	*DEV_UART_RX = 0;
    return c;
}

void * read_address() 
{
    /* Wait for address, write it to program_start and exit this loop */
    unsigned int addr = (unsigned int)uart_read_waiting() & 0xFF;
    /* TODO: check that address bytes come in the right order */
    for(int i = 1; i < 4; ++i) 
        addr |= ((unsigned int)uart_read_waiting() & 0xFF) << (8 * i);
    return reinterpret_cast<void*>(addr);
}

unsigned short int read_2b()  
{
    unsigned short int dl  = (unsigned short int)uart_read_waiting() & 0xFF;
                       dl |=((unsigned short int)uart_read_waiting() & 0xFF) << 8;
    return dl;
}

void uart_write_write_message_response(unsigned short int size) {
	unsigned char data[4] = { static_cast<unsigned char>(size & 0xFF),
								static_cast<unsigned char>(size >> 8), 0, 0};
	unsigned short crc = crc16(data, 2);
	data[2] = static_cast<unsigned char>(crc & 0xFF);
	data[3] = static_cast<unsigned char>(crc >> 8);
	uart_write_waiting(WRITE);
	for(int i = 0; i < 4; ++i)
		uart_write_waiting(data[i]);
}

void bootloader_main()
{
    unsigned char data[MAX_DATA_LENGTH + DATA_LENGTH_SIZE];
    unsigned char* ptr = &CODE_STARTS;

    while(1) {
        /* Wait for the next byte */
        char byte = uart_read_waiting();
        /* Answer to the PING message immediately */
        if(byte == PING) uart_write_waiting(OK_RESPONSE);
        /* Leave bootloader immediately after EXIT message */
        else if(byte == EXIT) {
            uart_write_waiting(OK_RESPONSE);
            _start();
        }
        /* Return CODE_STARTS symbol value */
        else if(byte == CODESTARTS) {
            unsigned int csi = reinterpret_cast<unsigned int>(&CODE_STARTS);
            data[0] = csi & 0xFF;
            data[1] = (csi >> 8)  & 0xFF;
            data[2] = (csi >> 16) & 0xFF;
            data[3] = (csi >> 24) & 0xFF;
            unsigned short crc = crc16(data, 4);
            data[4] = crc & 0xFF;
            data[5] = (crc >> 8) & 0xFF;
            uart_write_waiting(CODESTARTS);
            for(int i = 0; i < 7; ++i)
                uart_write_waiting(data[i]);
        }
        /* SETADDRESS message */
        else if(byte == SETADDRESS) {
        	unsigned char * tptr = static_cast<unsigned char*>(read_address());
        	unsigned int tptri = reinterpret_cast<unsigned int>(tptr);
        	data[0] = tptri & 0xFF;
        	data[1] = (tptri >> 8) & 0xFF;
        	data[2] = (tptri >> 16) & 0xFF;
        	data[3] = (tptri >> 24) & 0xFF;
        	unsigned short crc = crc16(data, 4);
        	unsigned short crc_received = read_2b();
       		uart_write_waiting(SETADDRESS);
        	if(crc == crc_received) {
        		ptr = tptr;
        		uart_write_waiting(OK_RESPONSE);
        	} else uart_write_waiting(DONT_UNDERSTAND);
        }
        /* Write message */
        else if(byte == WRITE) {
        	/* Receive data length */
            for(unsigned int i = 0; i < DATA_LENGTH_SIZE; ++i)
            	data[i] = uart_read_waiting();

            /* Produce data length out of received data */
            unsigned short data_length = ((unsigned short int)data[0]  & 0xFF) |
                                         ((unsigned short int)(data[1] & 0xFF) << 8);

			if(data_length > MAX_DATA_LENGTH) {
				uart_write_write_message_response(0);
				/* TODO: read all bytes, just not into memory */
             }

            for(unsigned int i = DATA_LENGTH_SIZE; 
                    i < data_length + DATA_LENGTH_SIZE; 
                    ++i) 
                data[i] = uart_read_waiting();

            unsigned short crc = read_2b(); 
            /* Entire message received now, calculate crc16 */
            /* in case crc16 is ok, write data to memory */
            if(crc == crc16(data, data_length + DATA_LENGTH_SIZE)) {
            	unsigned char *& address = ptr;
            	unsigned char * address_end = address + data_length;
                /* Check that address range is not overlapping bootloader address range */
                /* Start is in the bootloader address range */
                if(address >= BOOTLOADER_STARTS && address <= BOOTLOADER_ENDS)
                	uart_write_write_message_response(0);
                /* End is in the bootloader address range */
                else if(address_end >= BOOTLOADER_STARTS && address_end <= BOOTLOADER_ENDS)
                	uart_write_write_message_response(0);
                /* Address range contains bootloader address range */
                else if(address <= BOOTLOADER_STARTS && address_end >= BOOTLOADER_ENDS)
                	uart_write_write_message_response(0);
                else {
                	for(unsigned int i = DATA_LENGTH_SIZE; 
                        	i < data_length + DATA_LENGTH_SIZE; 
                        	++i) 
                    	*address++ = data[i];
                	uart_write_write_message_response(data_length);
                }
            }
            /* Corrupt data */
            else uart_write_write_message_response(0);
        }
        else if(byte == READ) {
            unsigned short int data_length = read_2b();
            if(data_length < MAX_DATA_LENGTH) data_length = MAX_DATA_LENGTH;
            	/* Fill buffer with the data and sent it */
            uart_write_waiting(READ);
            unsigned short crc = crc16(ptr, data_length);
            for(int i = 0; i < data_length; ++i)
            	uart_write_waiting(*ptr++);
            uart_write_waiting(crc & 0xFF);
            uart_write_waiting(crc >> 8);
        }
        /* Illegal message type */
        else uart_write_waiting(DONT_UNDERSTAND);
    }
}

