#include "mbed.h"
#include "crc.h"
#define CRC 0xFFFF

uint16_t crc16(uint8_t* data, uint8_t length){
	uint8_t temp;
	uint16_t crc = CRC;

	for (uint8_t i = 0; i < length; i++) {
			temp = crc >> 8 ^ data[i];
			temp ^= temp>>4;
			crc = (crc << 8) ^ ((uint16_t)(temp << 12)) ^ ((uint16_t)(temp <<5)) ^ ((uint16_t)temp);
	}
	return crc;
}
