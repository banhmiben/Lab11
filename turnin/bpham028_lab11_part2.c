/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <timer.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void transmit_data(unsigned char data) {
	int i;
	for (i = 0; i < 8; ++i) {
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
}

#define lightoff 0x00
#define lightstate1 0x01
#define lightstate2 0x02
#define lightstate3 0x03
unsigned char go = 0x00;
unsigned char tmpA;


enum CycleStates {init0, off, offwait, release, default_state, first_state, third_state};
int CycleSMTick(int state) {

	switch(state){
		case(init0):
			state = off;
			break;
		case(off):
			if (tmpA == 0x03) {
				state = release;
			} break;
		case(release):
			if (tmpA == 0x00) {
				state = default_state;
			} break;
		case(default_state):
			if (tmpA == 0x01) {
				state = third_state;
			} else if (tmpA == 0x02) {
				state = first_state;
			} else if (tmpA == 0x03) {
				state = offwait;
			} break;
		case(third_state):
			if (tmpA == 0x01) {
				state = first_state;
			} else if (tmpA == 0x02) {
				state = default_state;
			} else if (tmpA == 0x03) {
				state = offwait;
			} break;
		case(first_state):
			if (tmpA == 0x01) {
				state = default_state;
			} else if (tmpA == 0x02) {
				state = third_state;
			} else if (tmpA == 0x03) {
				state = offwait;
			} break;
		case(offwait):
			if (tmpA == 0x00) {
				state = off;
			} break;
		default:
			state = init0;
			break;
	}
	switch(state) {
		case(init0):
			go = lightoff;
			transmit_data(0x00);
			break;
		case(off):
			break;
		case(release):
			break;
		case(default_state):
			go = lightstate2;
			break;
		case(third_state):
			go = lightstate3;
			break;
		case(first_state):
			go = lightstate1;
			break;
		case(offwait):
			go = lightoff;
			transmit_data(0x00);
			break;
		default:
			break;
	}
	return state;
}


enum FestiveFirst {Init, One, Two, Three, Four}; //go == 0x01
int FestiveFirstSMTick(int state) {

	switch(state) {
		case(Init):
			if (go == lightstate1) {
				state = One;
			} break;
		case(One):
			if (go == lightstate1) {
				state = Two;
			} else {
				state = Init;
			} break;
		case(Two):
			if (go == lightstate1) {
				state = Three;
			} else {
				state = Init;
			} break;
		case(Three):
			if (go == lightstate1) {
				state = Four;
			} else {
				state = Init;
			} break;
		case(Four):
			if (go == lightstate1) {
				state = One;
			} else {
				state = Init;
			} break;
		default:
			state = Init;
			break;
	}
	switch(state) {
		case(Init):
			break;
		case(One):
			transmit_data(0x81); break;
		case(Two):
			transmit_data(0x42); break;
		case(Three):
			transmit_data(0x24); break;
		case(Four):
			transmit_data(0x18); break;
		default:
			break;
	}
	return state;
}

enum FestiveDefault {Init2, One2, Two2, Three2, Four2, Five, Six, Seven, Eight};
int FestiveDefaultSMTick(int state) {

	switch(state) {
		case(Init2):
			if (go == lightstate2) {
				state = One2;
			} break;
		case(One2):
			if (go == lightstate2) {
				state = Two2;
			} else {
				state = Init2;
			} break;
		case(Two2):
			if (go == lightstate2) {
				state = Three2;
			} else {
				state = Init2;
			} break;
		case(Three2):
			if (go == lightstate2) {
				state = Four2;
			} else {
				state = Init2;
			} break;
		case(Four2):
			if (go == lightstate2) {
				state = Five;
			} else {
				state = Init2;
			} break;
		case(Five):
			if (go == lightstate2) {
				state = Six;
			} else {
				state = Init2;
			} break;
		case(Six):
			if (go == lightstate2) {
				state = Seven;
			} else {
				state = Init2;
			} break;
		case(Seven):
			if (go == lightstate2) {
				state = Eight;
			} else {
				state = Init2;
			} break;
		case(Eight):
			if (go == lightstate2) {
				state = One2;
			} else {
				state = Init2;
			} break;
		default:
			state = Init2;
			break;
	}
	switch(state) {
		case(Init2):
			break;
		case(One2):
			transmit_data(0x01); break;
		case(Two2):
			transmit_data(0x02); break;
		case(Three2):
			transmit_data(0x04); break;
		case(Four2):
			transmit_data(0x08); break;
		case(Five):
			transmit_data(0x10); break;
		case(Six):
			transmit_data(0x20); break;
		case(Seven):
			transmit_data(0x40); break;
		case(Eight):
			transmit_data(0x80); break;
		default: break;
	}
	return state;
}

enum FestiveThird {Init1, One1, Two1, Three1, Four1}; //go == 0x02
int FestiveThirdSMTick(int state) {
	switch(state) {
		case(Init1):
			if (go == lightstate3) {
				state = One1;
			} break;
		case(One1):
			if (go == lightstate3) {
				state = Two1;
			} else {
				state = Init1;
			} break;
		case(Two1):
			if (go == lightstate3) {
				state = Three1;
			} else {
				state = Init1;
			} break;
		case(Three1):
			if (go == lightstate3) {
				state = Four1;
			} else {
				state = Init1;
			} break;
		case(Four1):
			if (go == lightstate3) {
				state = One1;
			} else {
				state = Init1;
			} break;
		default:
			state = Init1;
			break;
	}

	switch(state) {
		case(Init1):
			break;
		case(One1):
			transmit_data(0xA0); break;
		case(Two1):
			transmit_data(0x0A); break;
		case(Three1):
			transmit_data(0x50); break;
		case(Four1):
			transmit_data(0x05); break;
		default: break;
	}
	return state;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
    /* Insert your solution below */
	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;
	//Task1
	task1.state = start;
	task1.period = 200;
	task1.elapsedTime = task1.period;
	task1.TickFct = &CycleSMTick;
	//Task2
	task2.state = start;
	task2.period = 200;
	task2.elapsedTime = task2.period;
	task2.TickFct = &FestiveFirstSMTick;
	//Task3
	task3.state = start;
	task3.period = 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &FestiveThirdSMTick;
	//Task4
	task4.state = start;
	task4.period = 200;
	task4.elapsedTime = task4.period;
	task4.TickFct = &FestiveDefaultSMTick;

	TimerSet(200);
	TimerOn();

	unsigned short i; //scheduler loop iterator
	while(1) {
		tmpA = ~PINA;
		for (i = 0; i < numTasks; i++) {
			if (tasks[i] -> elapsedTime == tasks[i] -> period) {
				tasks[i] -> state = tasks[i] -> TickFct(tasks[i] -> state);
				tasks[i] -> elapsedTime = 0;
			}
			tasks[i] -> elapsedTime += 200;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;

}
