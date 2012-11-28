#include <system>

/* After bootloader starts, it is ready to accept several types
    of messages: ping, memory write/read, exit to the main code

    Each message starts with the message type - 1 byte.

    * Ping message
    [TO BOOTLOADER]     MsgType ('?')
    [FROM BOOTLOADER]   MsgType ('!')

    * Memory write/read message 
    [TO BOOTLOADER]     MsgType (0) | (Address, 4 bytes) | (SizeInBytes, 2 bytes) | (Data) | (CRC16 of all message after MsgType, 2 bytes)
    [FROM BOOTLOADER]   MsgType (0) | (SizeInBytesWritten, 2 bytes)
        In case any errors, SizeInBytesWritten will be less than SizeInBytes (requested to write), 
        so the operation should be repeated.
    [TO BOOTLOADER]     MsgType (1) | (Address, 4 bytes) | (SizeInBytes, 2 bytes)
    [FROM BOOTLOADER]   MsgType (1) | (Data) | (CRC16 of data, 2 bytes)

    * Exit to main code message *
    [TO BOOTLOADER]     MsgType ('@')

    * Illegal message (unknown type), immediately after the first illegal byte received after
        leaving awaiting-for-message state
    [FROM BOOTLOADER]  '?'
*/

enum BootloaderMessageType {
    WRITE,
    READ,
    EXIT,
    EXIT_DEFAULT = '@',
    PING = '?'
};

constexpr static unsigned int ADDRESS_SIZE = 4;
constexpr static unsigned int DATA_LENGTH_SIZE = 2;
constexpr static unsigned int MAX_DATA_LENGTH = 32;
constexpr static char PING_RESPONSE = '!';
constexpr static char DONT_UNDERSTAND = '?';

/* We need to get this function inlined for bootloader to be fully independend of
    any other code, which might get overriden during "flash" process */

#define BOOTLOADER __attribute__((section(".bootloader")))

unsigned short crc16(unsigned char *pcBlock, unsigned short len) BOOTLOADER;
void uart_write_waiting(char x) BOOTLOADER;
char uart_read_waiting() BOOTLOADER;
void uart_write_write_message_response(unsigned short int size) BOOTLOADER;
void * read_address() BOOTLOADER;
unsigned short int read_2b() BOOTLOADER;

extern char* BOOTLOADER_STARTS;
extern char* BOOTLOADER_ENDS;

unsigned short crc16(unsigned char *pcBlock, unsigned short len)
{
    unsigned short crc = 0xFFFF;
    unsigned char i;

    while (len--) {
        crc ^= *pcBlock++ << 8;
        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

void uart_write_waiting(char x) 
{
    while(System::Bus::UART::TX_BUFFER_LENGTH - *DEV_UART_TX == 0) { asm("nop");  }
    *DEV_UART_TX = x;
}

char uart_read_waiting() 
{
    while((*DEV_UART_RX >> 16) != 0) { asm("nop"); }
	char c = *DEV_UART_RX & 0xFF;
	*DEV_UART_RX = 0;
    return c;
}

void * read_address() 
{
    /* Wait for address, write it to program_start and exit this loop */
    void * ptr;
    unsigned int addr = 0;
    /* TODO: check that address bytes come in the right order */
    for(int i = 0; i < 4; ++i) 
        addr |= (unsigned int)uart_read_waiting() << (8 * i);
    ptr = reinterpret_cast<void*>(addr);
    return ptr;
}

unsigned short int read_2b()  
{
    /* Wait for address, write it to program_start and exit this loop */
    unsigned short int dl = 0;
    /* TODO: check that address bytes come in the right order */
    for(int i = 0; i < 2; ++i) 
        dl |= (unsigned short int)uart_read_waiting() << (8 * i);
    return dl;
}

void uart_write_write_message_response(unsigned short int size) {
	uart_write_waiting(WRITE);
	uart_write_waiting(size & 0xFF);
	uart_write_waiting(size >> 8);
}

void bootloader_main()
{
    unsigned char data[MAX_DATA_LENGTH + DATA_LENGTH_SIZE + ADDRESS_SIZE];

    while(1) {
        /* Wait for the next byte */
        char byte = uart_read_waiting();
        /* Answer to the PING message immediately */
        if(byte == PING) uart_write_waiting(PING_RESPONSE);
        /* Leave bootloader immediately after EXIT_DEFAULT message */
        else if(byte == EXIT) _start();
        /* Write message */
        else if(byte == WRITE) {
            /* Read write address and data length into the buffer,
             because we need to calculate CRC later */
            for(unsigned int i = 0; i < ADDRESS_SIZE + DATA_LENGTH_SIZE; ++i) 
                data[i] = uart_read_waiting();

            /* Produce data length out of received data */
            unsigned short data_length = (unsigned short int)data[ADDRESS_SIZE] | 
                                         (unsigned short int)(data[ADDRESS_SIZE + 1] << 8);

            for(unsigned int i = ADDRESS_SIZE + DATA_LENGTH_SIZE; 
                    i < data_length + ADDRESS_SIZE + DATA_LENGTH_SIZE; 
                    ++i) 
                data[i] = uart_read_waiting();

            unsigned short crc = read_2b();
            if(crc == crc16(data, data_length + ADDRESS_SIZE + DATA_LENGTH_SIZE)) {
                /* Produce memory address out of received data */
                unsigned int ptri = (unsigned int)data[0] | 
                                    (unsigned int)(data[1] << 8) | 
                                    (unsigned int)(data[2] << 16) | 
                                    (unsigned int)(data[3] < 24);
                char * address = reinterpret_cast<char*>(ptri);
                char * address_end = address + data_length;
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
                	/* Entire message received now, calculate crc16 */
                	/* in case crc16 is ok, write data to memory */
                	for(unsigned int i = ADDRESS_SIZE + DATA_LENGTH_SIZE; 
                        	i < data_length + ADDRESS_SIZE + DATA_LENGTH_SIZE; 
                        	++i) 
                    	*address++ = data[i];
                	uart_write_write_message_response(data_length);
                }
            }
            /* Corrupt data */
            else uart_write_write_message_response(0);
        }
        else if(byte == READ) {
            unsigned char * ptr = reinterpret_cast<unsigned char*>(read_address());
            unsigned short int data_length = read_2b();
            if(data_length < MAX_DATA_LENGTH) data_length = MAX_DATA_LENGTH;
            	/* Fill buffer with the data and sent it */
            uart_write_waiting(READ);
            unsigned short int crc = crc16(ptr, data_length);
            for(int i = 0; i < data_length; ++i)
            	uart_write_waiting(ptr[i]);
            uart_write_waiting(crc & 0xFF);
            uart_write_waiting(crc >> 8);
        }
        /* Illegal message type */
        else uart_write_waiting(DONT_UNDERSTAND);
    }
}

