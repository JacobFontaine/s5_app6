/*********************************************
Noms : Philippe Garneau et Jacob Fontaine
CIP : garp2405 et fonj1903
Date : 2 avril 2018

Description: Code du Noeud Coordinateur
*********************************************/

#include "mbed.h"

#define CRC 0xFFFF

PwmOut led(LED2);
PwmOut pin(p21);
InterruptIn  pinterrupt(p14);
int beginValue=0;
int endValue=0;
Timer timeBoi;
uint8_t message[33] = "Bonjour la gang";
uint8_t trameInitale[40];
uint8_t trameManchester[80];

uint8_t data[6] = {0x61, 0x6c, 0x6c, 0x6f, 0x00, 0x00};
uint8_t dataTest[8] = {0x61, 0x6c, 0x6c, 0x6f, 0x00, 0x01, 0x27, 0xE5};

static void insertHeader()
{
	trameInitale[2] = 0b00000000;
	trameInitale[3] = 0x21;
}
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


uint8_t get_bit(uint8_t bits, uint8_t pos)
{
   if(((bits >> (7-pos)) & 0x01)==1)
		 return 0b10; 
	 else
		 return 0b01;
}

static void encodageManchester()
{
	for(uint8_t i =0; i<40; i++)
	{	
		trameManchester[i*2] = (((((get_bit(trameInitale[i],0)<<2) + get_bit(trameInitale[i],1)) << 2)
													+ get_bit(trameInitale[i],2)) << 2) + get_bit(trameInitale[i],3);
		
		trameManchester[i*2+1] = (((((get_bit(trameInitale[i],4)<<2) + get_bit(trameInitale[i],5)) << 2)
													+ get_bit(trameInitale[i],6)) << 2) + get_bit(trameInitale[i],7);
	}
}


static void insertMessage()
{
	for(uint8_t index =0;index<33;index++)
	{
		trameInitale[index+4] = message[index];
	}
}

static void insertCRC()
{
	uint16_t crcResult = crc16(message,33);
	trameInitale[37] = crcResult >> 8;
	trameInitale[38] =	crcResult & 0xff;
}
int main()
{ 
	//Préambule	
	trameInitale[0]= 0b01010101;
	//Start
	trameInitale[1]= 0b01111110;
	//En tête
	insertHeader();
	//Message
	insertMessage();
	//CRC16
	insertCRC();
	//End
	trameInitale[39]= 0b01111110;
	
	//Ecodage Manchester
	encodageManchester();
	


}
