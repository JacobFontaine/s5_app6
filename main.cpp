/*********************************************
Noms : Philippe Garneau et Jacob Fontaine
CIP : garp2405 et fonj1903
Date : 2 avril 2018

Description: Code du Noeud Coordinateur
*********************************************/

#include "mbed.h"
#include "FrameBuilder.h"
#include "crc.h"

DigitalOut cereal(p9);
InterruptIn  pinterrupt(p14);
DigitalIn input(p14);
Timer timeBoi;
InterruptIn buttonInterrupt(p8);
Thread sendThread;
DigitalOut led(LED1);

int beginValue=0;
int endValue=0;

uint8_t message[66] = "Bonjour la gangbbbbbbbbbbbbbbbbbbk";
uint8_t trameManchester[80];

bool rxStandby = true;
bool rxSync = false;
bool incompleteByte = true;
bool rxStart = false;
bool rxHeader = false;
uint16_t rxRate = 0;
uint16_t counter = 0;
uint8_t bitCounter = 0;
uint8_t byte = 0;
uint8_t frameHeader[2] = {};
uint8_t trameRebuild[37] = {};
uint8_t trameRebuildCounter = 0;
uint8_t finalFrame[35] = {};


void rebuildMessage(uint8_t bit) {
	
	if (incompleteByte) {
		//printf("bit received: %d\n\r", bit);
		byte <<= 1;
		byte += bit;
		bitCounter++;
		//printf("Byte: %02x\n\r", byte);
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
				for (int i = 2; i < 35; i++) {
					printf("%c", trameRebuild[i]);
				}
				printf("\n\r");
//				printf("%s\n", trameRebuild);
				if (crc16(trameRebuild + 2, 35) != 0)
					led = 1;
				else
					led = 0;
				// I AM DONE
				rxStandby = true;
				rxRate = 0;
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
		counter++;
		timeBoi.reset();
	}
	else if ((0.8 * rxRate) < timeBoi.read_ms()) {
		timeBoi.reset();
		rebuildMessage(0);
	}
}

void fall()
{
	if (rxStandby) return;
	
	else if (rxSync)
	{
		rxRate += timeBoi.read_ms();
		counter++;
		timeBoi.reset();
		
		if (counter == 7)
		{
			rxRate /= 7;
			rxSync = false;
//			printf("WE ARE READY TO READ\n\r RX RATE: %d \n\r", rxRate);
			counter = 0;
		}
	}
	
	else if ((0.8 * rxRate) < timeBoi.read_ms()) {
		timeBoi.reset();
		rebuildMessage(1);
	}
}

void sendData() {
	uint16_t leftover = 0;
	
	while(1) {
		sendThread.signal_wait(0x01);
		
		do {
			if (leftover > 0) {
				leftover = buildFrame(message + leftover, trameManchester, leftover);
			}
			else {
				leftover = buildFrame(message, trameManchester, sizeof(message));
			}
			
			for (int i = 0; i < 80; i++) {
				for (int index = 0; index < 8; index++) {
					int penis = ((trameManchester[i] >> (7-index)) & 0x01);
					cereal = penis;
					wait_ms(10);
				}
			}
			cereal = 0;
			wait_ms(200);
		} while (leftover > 0);
	}
}

void checkButton() {
	sendThread.signal_set(0x01);
}	

int main()
{ 
	sendThread.start(&sendData);
	
	wait(2);
	
	buttonInterrupt.rise(&checkButton);
	
	pinterrupt.rise(&rise);
	pinterrupt.fall(&fall);
}
