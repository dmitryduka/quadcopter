#ifndef CRC16_HPP
#define CRC16_HPP

unsigned short crc16(const unsigned char *pcBlock, unsigned short len)
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

#endif
