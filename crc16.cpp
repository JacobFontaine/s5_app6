#include "mbed.h"
#include <list>

uint8_t test[35] =
uint8_t crcPly[3] = {0x1, 0x10, 0x21};
uint8_t crcPoly[17] = {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

uint8_t data[12] = {0xA0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

uint16_t crc16(const unsigned char* data_p, uint8_t length){
	uint8_t x;
	uint16_t crc = 0xFFFF;

	for (uint8_t i = 0; i < length; i++) {
			x = crc >> 8 ^ *data_p++;
			x ^= x>>4;
			crc = (crc << 8) ^ ((uint16_t)(x << 12)) ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
	}
	return crc;
}

