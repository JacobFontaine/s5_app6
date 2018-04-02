/*********************************************
Noms : Philippe Garneau et Jacob Fontaine
CIP : garp2405 et fonj1903
Date : 2 avril 2018

Description: Code du Noeud Coordinateur
*********************************************/

#include "mbed.h"

#define CRC 0xFFFF

DigitalOut cereal(p9);
InterruptIn  pinterrupt(p14);
DigitalIn input(p14);
Timer timeBoi;

int beginValue=0;
int endValue=0;

uint8_t message[33] = "Bonjour la gang";
uint8_t trameInitale[40];
uint8_t trameManchester[80];

bool rxStandby = true;
bool rxSync = false;
bool incompleteByte = true;
bool rxStart = false;
uint16_t rxRate = 0;
uint16_t counter = 0;
uint8_t bitCounter = 0;
uint8_t byte = 0;
uint8_t trameRebuild[36] = {};
uint8_t trameRebuildCounter = 0;


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

void rebuildMessage(uint8_t bit) {
	
	if (incompleteByte) {
		printf("bit received: %d\n\r", bit);
		byte <<= 1;
		byte += bit;
		bitCounter++;
		printf("Byte: %02x\n\r", byte);
		if (bitCounter == 8) {
			incompleteByte = false;
		}
	}
	if (!incompleteByte) {
		if (byte == 0b01111110) {
			if (!rxStart)
				rxStart = true;
			
			else {
				rxStart = false;
				trameRebuildCounter = 0;
				// I AM DONE
			}
		}
		
		else if (rxStart) {
			
			trameRebuild[trameRebuildCounter] = byte;
			trameRebuildCounter++;
		}
		byte = 0;
		bitCounter = 0;
		incompleteByte = true;
	}
}

void rise()
{
	printf("RISE\n\r");
	if (rxStandby)
	{
		rxStandby = false;
		rxSync = true;
		timeBoi.start();
		timeBoi.reset();
	}
	
	else if (rxSync)
	{
		rxRate += timeBoi.read_ms();
		printf("RXRATEREAD: %d\n\r", rxRate);
		counter++;
		printf("COUNTER: %d\n\r", counter);
		timeBoi.reset();
	}
	else if (timeBoi.read_ms() > (1	* rxRate)) {
		timeBoi.reset();
		rebuildMessage(0);
	}
}

void fall()
{
	printf("FALL\n\r");
	if (rxStandby) return;
	
	else if (rxSync)
	{
		rxRate += timeBoi.read_ms();
		printf("RXRATEREAD: %d\n\r", rxRate);
		counter++;
		printf("COUNTER: %d\n\r", counter);
		timeBoi.reset();
		
		if (counter == 7)
		{
			rxRate /= 7;
			rxSync = false;
			printf("WE ARE READY TO READ\n\r RX RATE: %d \n\r", rxRate);
			counter = 0;
		}
	}
	
	else if (timeBoi.read_ms() > (1 * rxRate)) {
		timeBoi.reset();
		rebuildMessage(1);
	}
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
	
	pinterrupt.rise(&rise);
	pinterrupt.fall(&fall);
	
	for (int i = 0; i < 80; i++) {
		for (int index = 0; index < 8; index++) {
			int penis = ((trameManchester[i] >> (7-index)) & 0x01);
			cereal = penis;
			wait_ms(50);
		}
	}
	for (int i = 0; i < 37; i++) {
		printf("%02x\n\r", trameRebuild[i]);
	}
}
