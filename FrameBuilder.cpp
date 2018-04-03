#include "mbed.h"
#include "crc.h"
#include "FrameBuilder.h"

uint8_t trameInitiale[40];

static void insertHeader(size_t size)
{
	trameInitiale[2] = 0b00000000;
	trameInitiale[3] = size;
}

uint8_t get_bit(uint8_t bits, uint8_t pos)
{
   if(((bits >> (7-pos)) & 0x01)==1)
		 return 0b10; 
	 else
		 return 0b01;
}

static void encodageManchester(uint8_t *trameManchester)
{
	for(uint8_t i =0; i<40; i++)
	{	
		trameManchester[i*2] = (((((get_bit(trameInitiale[i],0)<<2) + get_bit(trameInitiale[i],1)) << 2)
													+ get_bit(trameInitiale[i],2)) << 2) + get_bit(trameInitiale[i],3);
		
		trameManchester[i*2+1] = (((((get_bit(trameInitiale[i],4)<<2) + get_bit(trameInitiale[i],5)) << 2)
													+ get_bit(trameInitiale[i],6)) << 2) + get_bit(trameInitiale[i],7);
	}
}


static uint16_t insertMessage(uint8_t *message, size_t size)
{
	uint8_t index = 0;
	for(index = 0;index<33;index++)
	{
		trameInitiale[index+4] = message[index];
	}
	return (size - index);
}

static void insertCRC(uint8_t *message)
{
	uint16_t crcResult = crc16(message,33);
	trameInitiale[37] = crcResult >> 8;
	trameInitiale[38] =	crcResult & 0xff;
}

uint16_t buildFrame(uint8_t *message, uint8_t *manchesterFrame, size_t size) {
	memset(manchesterFrame, 0, 80);
	// Preambule
	trameInitiale[0]= 0b01010101;
	
	//Start
	trameInitiale[1]= 0b01111110;
	
	//En tête
	insertHeader(size);
	//Message
	uint16_t leftover = insertMessage(message, size);

	//CRC16
	insertCRC(message);
	//End
	trameInitiale[39]= 0b01111110;

	//Ecodage Manchester
	encodageManchester(manchesterFrame);
	
	return leftover;
}
