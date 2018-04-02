/*********************************************
Noms : Axel Bosco et Jacob Fontaine
CIP : bosa2002 et fonj1903
Date : 13 février 2018

Description: Code du Noeud Coordinateur
*********************************************/

#include "mbed.h"

#define CRC 0xFFFF

uint8_t data[6] = {0x61, 0x6c, 0x6c, 0x6f, 0x00, 0x00};
uint8_t dataTest[8] = {0x61, 0x6c, 0x6c, 0x6f, 0x00, 0x01, 0x27, 0xE5};

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

int main() {  

	while(1) {
		printf("crc is: %04x\n\r", crc16(data, 6));
		wait(1);
		printf("Remainder is: %04x\n\r", crc16(dataTest, 8));
		wait(0.5);
	}
}
