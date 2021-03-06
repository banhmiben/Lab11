#include <avr/io.h>
#include <timer.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


void transmit_data(unsigned short data) {
	int i;
	for (i = 0; i < 16; ++i) {
		//Sets SRCLR to 1 allowing data to be set 
		//Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		//set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		//set SRCLK = 1. Rising edge shifts next bit of data into the shift register 
		PORTC |= 0x02;
	}
	//set RCLK = 1. Rising edge copies data from "Shift" register to "Storage" register
	PORTC |= 0x04;
	//clears all lines in preparation of a new transmission
	PORTC = 0x00;
	//additional pulse
} 

enum States {Start, Init, Wait, WaitInc ,Inc, WaitDec, Dec, Reset} State;

unsigned char tmpA;
unsigned short d = 0x0000;
void Tick() {

	tmpA = ~PINA & 0x07;
	switch(State) {
		case Start:
			State = Init;
			break;
		case Init:
			State = Wait;
			break;
		case Wait:
			if (tmpA == 0x03) {
				State = Reset;
			} else if (tmpA == 0x01) {
				State = WaitInc;
			} else if (tmpA == 0x02) {
				State = WaitDec;
			} break;
		case WaitInc:	
			State = Inc;
			break;
		case Inc:
			if (tmpA == 0x01) {
				State = Inc;
			} else {
				State = Wait;
			} break;
		case WaitDec:
			State = Dec;
			break;
		case Dec:
			if (tmpA == 0x02) {
				State = Dec;
			} else {
				State = Wait;
			} break;
		case Reset:
			if (tmpA == 0x03) {
				State = Reset;
			} else {
				State = Wait;
			} break;
		default:
			State = Start;
			break;
	}

	switch(State) {
		case Start:
			break;
		case Init:
			d = 0x02FF;
			break;
		case Wait:
			break;
		case Inc:
			break;
		case Dec:
			break;
		case WaitInc:
			if (d < 0xFFFF) {
				++d;
			} break;
		case WaitDec:
			if (d > 0x0000) {
				--d;
			} break;
		case Reset:
			d = 0x0000;
			break;
		default:
			d = 0x0000;
			break;
	}
	transmit_data(d);

}


int main(void) {
    /* Insert DDR and PORT initializations */
		DDRA = 0x00; PORTA = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
    /* Insert your solution below */
    while (1) {
		Tick();
    }
    return 1;
}
