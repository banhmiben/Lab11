/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <timer.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char go = 0x00;
unsigned char tmpA;

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

enum CycleStates {Start, CInit, Wait, Power, Release, WaitUp, Up, WaitDown, Down};
int CycleSMTick(int state) {
	
	tmpA = ~PINA & 0x07;
	switch(state) {
		case(Start): state = CInit; break;
		case(CInit): state = Wait; break;
		case(Wait):
			if (tmpA == 0x03) {
				state = Power;
			} else {
				state = Wait;
			} break;
		case(Power):
			state = Release;
			break;
		case(Release):
			if (tmpA == 0x03) {
				state = CInit;
			} else if (tmpA == 0x01) {
				state = WaitUp;
			} else if (tmpA == 0x02) {
				state = WaitDown;
			} break;
		case(WaitUp):
			state = Up; break;
		case(Up): 
			if (tmpA == 0x01) {
				state = Up;
			} else {
				state = Release;
			} break;
		case(WaitDown):
			state = Down; break;
		case(Down):
			if (tmpA == 0x02) {
				state = Down;
			} else {
				state = Release;
			} break;
		default: state = Start; break;
	}
	switch(state) {
		case(Start): break;
		case(CInit): 
			go = 0x00;
			transmit_data(0x00);
			break;
		case(Wait): break;
		case(Power):
			go = 0x01;
			break;
		case(Release): break;
		case(WaitUp):
			if (go == 0x01) {
				go = 0x02;
			} else if (go == 0x02) {
				go = 0x03;
			} else if (go == 0x03) {
				go = 0x01;
			} break;
		case(Up): break;
		case(WaitDown):
			if (go == 0x01) {
				go = 0x03;
			} else if (go == 0x02) {
				go = 0x01;
			} else if (go == 0x03) {
				go = 0x02;
			} break;
		case(Down): break;
		default: break;
	}
	return state;
}

enum FestiveOne {Start1, Init, One, Two, Three, Four}; //go == 0x01
int FestiveOneSMTick(int state) {
	
	switch(state) {
		case(Start1): state = Init; break;
		case(Init): 
			if (go == 0x01) {
				state = One;
			} break;
		case(One): state = Two; break;
		case(Two): state = Three; break;
		case(Three): state = Four; break;
		case(Four): state = One; break;
		default: state = Init; break;
	}
	switch(state) {
		case(Init):
			transmit_data(0x00);
			break;
		case(One): transmit_data(0x81); break;
		case(Two): transmit_data(0x42); break;
		case(Three): transmit_data(0x24); break;
		case(Four): transmit_data(0x18); break;
		default: break;
	}
	return state;
}

enum FestiveTwo {Start2, Init1, One1, Two1, Three1, Four1}; //go == 0x02
int FestiveTwoSMTick(int state) {
	switch(state) {
		case(Start2): state = Init1; break;
		case(Init1): 
			if (go == 0x02) {
				state = One1;
			} break;
		case(One1): state = Two1; break;
		case(Two1): state = Three1; break;
		case(Three1): state = Four1; break;
		case(Four1): state = One1; break;
		default: state = Init1; break;
	}
	switch(state) {
		case(Init):
			transmit_data(0x00);
			break;
		case(One1): transmit_data(0xA0); break;
		case(Two1): transmit_data(0x0A); break;
		case(Three1): transmit_data(0x50); break;
		case(Four1): transmit_data(0x05); break;
		default: break;
	}
	return state;
}

enum FestiveThree {Start3, Init2, One2, Two2, Three2, Four2, Five, Six, Seven, Eight};
int FestiveThreeSMTick(int state) {
	switch(state) {
		case(Start3): state = Init2;
		case(Init2): 
			if (go == 0x02) {
				state = One2;
			} break;
		case(One2): state = Two2; break;
		case(Two2): state = Three2; break;
		case(Three2): state = Four2; break;
		case(Four2): state = Five; break;
		case(Five): state = Six; break;
		case(Six): state = Seven; break;
		case(Seven): state = Eight; break;
		case(Eight): state = One2; break;
		default: state = Init; break;
	}
	switch(state) {
		case(Init2):
			transmit_data(0x00);
			break;
		case(One2): transmit_data(0x01); break;
		case(Two2): transmit_data(0x02); break;
		case(Three2): transmit_data(0x04); break;
		case(Four2): transmit_data(0x08); break;
		case(Five): transmit_data(0x10); break;
		case(Six): transmit_data(0x20); break;
		case(Seven): transmit_data(0x40); break;
		case(Eight): transmit_data(0x80); break;
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
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &CycleSMTick;
	//Task2
	task2.state = start;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &FestiveOneSMTick;
	//Task3
	task3.state = start;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &FestiveTwoSMTick;
	//Task4
	task4.state = start;
	task4.period = 100;
	task4.elapsedTime = task4.period;
	task4.TickFct = &FestiveThreeSMTick;
	
	TimerSet(100);
	TimerOn();
	
	unsigned short i; //scheduler loop iterator
	while(1) {
		for (i = 0; i < numTasks; i++) {
			if (tasks[i] -> elapsedTime == tasks[i] -> period) {
				tasks[i] -> state = tasks[i] -> TickFct(tasks[i] -> state);
				tasks[i] -> elapsedTime = 0;
			} 
			tasks[i] -> elapsedTime += 100;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
	
}
