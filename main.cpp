/*********************************************
Noms : Axel Bosco et Jacob Fontaine
CIP : bosa2002 et fonj1903
Date : 13 février 2018

Description: Code du Noeud Coordinateur
*********************************************/

#include "mbed.h"


PwmOut led(LED2);
PwmOut pin(p21);
InterruptIn  pinterrupt(p14);
int beginValue=0;
int endValue=0;
Timer timeBoi;


void functionBoiRise()
{
		//printf("%d\n\r",endValue-beginValue);
		timeBoi.start();
}
void functionBoiFall()
{
			timeBoi.stop();
			printf("%d\n\r",timeBoi.read_us());
			timeBoi.reset();
}
int main()
{  
	led.period(1.0);  // 4 second period
  led.write(0.1);
	pin.period(1.0);  // 4 second period
  pin.write(0.1);
		pinterrupt.rise(&functionBoiRise);
		pinterrupt.fall(&functionBoiFall);
	while(1);
}
