#include <avr/io.h>
#include <timer.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define lightoff 0
#define lightstate1 0x01
#define lightstate2 0x02
#define lightstate3 0x03
#define shift1 0x01
#define shift2 0x02

unsigned char go = 0x00;
unsigned char go2 = 0x00;
unsigned char tmpA;

void transmit_data(unsigned char data, unsigned char shiftNum) {
	int i;
	for (i = 0; i < 8; ++i) {
		//Sets SRCLR to 1 allowing data to be set 
		//Also clears SRCLK in preparation of sending data (shift1 SRCLR = 0x08 shift2 SRCLR = 0x20)
		if (shiftNum == shift1) {
			PORTC = 0x08;
		} else if (shiftNum == shift2) {
			PORTC = 0x20;
		}
		//set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		//set SRCLK = 1. Rising edge shifts next bit of data into the shift register 
		PORTC |= 0x02;
	}
	//set RCLK = 1. Rising edge copies data from "Shift" register to "Storage" register
	if (shiftNum == shift1) {
		PORTC |= 0x04;
	} else if (shiftNum == shift2) {
		PORTC |= 0x10;
	}
	//clears all lines in preparation of a new transmission
	PORTC = 0x00;
} 

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
			transmit_data(0x00, shift1);
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
			transmit_data(0x00, shift1);
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
			transmit_data(0x81, shift1); break;
		case(Two): 
			transmit_data(0x42, shift1); break;
		case(Three): 
			transmit_data(0x24,shift1); break;
		case(Four): 
			transmit_data(0x18, shift1); break;
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
			transmit_data(0x01, shift1); break;
		case(Two2):
			transmit_data(0x02, shift1); break;
		case(Three2):
			transmit_data(0x04, shift1); break;
		case(Four2):
			transmit_data(0x08, shift1); break;
		case(Five):
			transmit_data(0x10, shift1); break;
		case(Six):
			transmit_data(0x20, shift1); break;
		case(Seven):
			transmit_data(0x40, shift1); break;
		case(Eight):
			transmit_data(0x80, shift1); break;
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
			transmit_data(0xA0, shift1); break;
		case(Two1): 
			transmit_data(0x0A, shift1); break;
		case(Three1): 
			transmit_data(0x50, shift1); break;
		case(Four1): 
			transmit_data(0x05, shift1); break;
		default: break;
	}
	return state;
}

enum CycleStates2 {init3, off1, off1wait, release1, default_state1, first_state1, third_state1};
int Cycle2SMTick(int state) {
	
	switch(state){
		case(init3):
			state = off1;
			break;
		case(off1):
			if (tmpA == 0x0C) {
				state = release1;
			} break;
		case(release1):
			if (tmpA == 0x00) {
				state = default_state1;
			} break;
		case(default_state1):
			if (tmpA == 0x04) {
				state = third_state1;
			} else if (tmpA == 0x08) {
				state = first_state1;
			} else if (tmpA == 0x0C) {
				state = off1wait;
			} break;
		case(third_state1):
			if (tmpA == 0x04) {
				state = first_state1;
			} else if (tmpA == 0x08) {
				state = default_state1;
			} else if (tmpA == 0x0C) {
				state = off1wait;
			} break;
		case(first_state1):
			if (tmpA == 0x04) {
				state = default_state1;
			} else if (tmpA == 0x08) {
				state = third_state1;
			} else if (tmpA == 0x0C) {
				state = off1wait;
			} break;
		case(off1wait):
			if (tmpA == 0x00) {
				state = off1;
			} break;
		default:
			state = init3;
			break;
	}
	switch(state) {
		case(init3):
			go2 = lightoff;
			transmit_data(0x00, shift2);
			break;
		case(off1):
			break;
		case(release1):
			break;
		case(default_state1):
			go2 = lightstate2;
			break;
		case(third_state1):
			go2 = lightstate3;
			break;
		case(first_state1):
			go2 = lightstate1;
			break;
		case(off1wait):
			go2 = lightoff;
			transmit_data(0x00, shift2);
			break;
		default: 
			break;
	}
	return state;
}
		
			
enum FestiveFirst2 {Init4, One4, Two4, Three4, Four4}; //go == 0x01
int Festive2FirstSMTick(int state) {
	
	switch(state) {
		case(Init4): 
			if (go2 == lightstate1) {
				state = One4;
			} break;
		case(One4): 
			if (go2 == lightstate1) {
				state = Two4;
			} else {
				state = Init4;
			} break;
		case(Two4): 
			if (go2 == lightstate1) {
				state = Three4;
			} else {
				state = Init4;
			} break;
		case(Three4):
			if (go2 == lightstate1) {
				state = Four4;
			} else {
				state = Init4;
			} break;
		case(Four4):
			if (go2 == lightstate1) {
				state = One4;
			} else {
				state = Init4;
			} break;
		default:
			state = Init4;
			break;
	}
	switch(state) {
		case(Init4):
			break;
		case(One4): 
			transmit_data(0x81, shift2); break;
		case(Two4): 
			transmit_data(0x42, shift2); break;
		case(Three4): 
			transmit_data(0x24, shift2); break;
		case(Four4): 
			transmit_data(0x18, shift2); break;
		default:
			break;
	}
	return state;
}

enum FestiveDefault2 {Init5, One5, Two5, Three5, Four5, Five5, Six5, Seven5, Eight5};
int Festive2DefaultSMTick(int state) {
	
	switch(state) {
		case(Init5): 
			if (go2 == lightstate2) {
				state = One5;
			} break;
		case(One5): 
			if (go2 == lightstate2) {
				state = Two5;
			} else {
				state = Init5;
			} break;
		case(Two5): 
			if (go2 == lightstate2) {
				state = Three5;
			} else {
				state = Init5;
			} break;
		case(Three5):
			if (go2 == lightstate2) {
				state = Four5;
			} else {
				state = Init5;
			} break;
		case(Four5):
			if (go2 == lightstate2) {
				state = Five5;
			} else {
				state = Init5;
			} break;
		case(Five5): 
			if (go2 == lightstate2) {
				state = Six5;
			} else {
				state = Init5;
			} break;
		case(Six5): 
			if (go2 == lightstate2) {
				state = Seven5;
			} else {
				state = Init5;
			} break;
		case(Seven5):
			if (go2 == lightstate2) {
				state = Eight5;
			} else {
				state = Init5;
			} break;
		case(Eight5):
			if (go2 == lightstate2) {
				state = One5;
			} else {
				state = Init5;
			} break;
		default:
			state = Init5;
			break;
	}
	switch(state) {
		case(Init5):
			break;
		case(One5):
			transmit_data(0x01, shift2); break;
		case(Two5):
			transmit_data(0x02, shift2); break;
		case(Three5):
			transmit_data(0x04, shift2); break;
		case(Four5):
			transmit_data(0x08, shift2); break;
		case(Five5):
			transmit_data(0x10, shift2); break;
		case(Six5):
			transmit_data(0x20, shift2); break;
		case(Seven5):
			transmit_data(0x40, shift2); break;
		case(Eight5):
			transmit_data(0x80, shift2); break;
		default: break;
	}
	return state;
}

enum FestiveThird2 {Init6, One6, Two6, Three6, Four6}; //go == 0x02
int Festive2ThirdSMTick(int state) {
	switch(state) {
		case(Init6): 
			if (go2 == lightstate3) {
				state = One6;
			} break;
		case(One6): 
			if (go2 == lightstate3) {
				state = Two6;
			} else {
				state = Init6;
			} break;
		case(Two6): 
			if (go2 == lightstate3) {
				state = Three6;
			} else {
				state = Init6;
			} break;
		case(Three6):
			if (go2 == lightstate3) {
				state = Four6;
			} else {
				state = Init6;
			} break;
		case(Four6):
			if (go2 == lightstate3) {
				state = One6;
			} else {
				state = Init6;
			} break;
		default:
			state = Init6;
			break;
	}

	switch(state) {
		case(Init6):
			break;
		case(One6): 
			transmit_data(0xA0, shift2); break;
		case(Two6): 
			transmit_data(0x0A, shift2); break;
		case(Three6): 
			transmit_data(0x50, shift2); break;
		case(Four6): 
			transmit_data(0x05, shift2); break;
		default: break;
	}
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
		DDRA = 0x00; PORTA = 0xFF;
        DDRC = 0xFF; PORTC = 0x00;
    /* Insert your solution below */
	static task task1, task2, task3, task4, task5, task6, task7, task8;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8};
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
	//Task5
	task5.state = start;
	task5.period = 200;
	task5.elapsedTime = task5.period;
	task5.TickFct = &Cycle2SMTick;
	//Task6
	task6.state = start;
	task6.period = 200;
	task6.elapsedTime = task6.period;
	task6.TickFct = &Festive2FirstSMTick;
	//Task7
	task7.state = start;
	task7.period = 200;
	task7.elapsedTime = task7.period;
	task7.TickFct = &Festive2ThirdSMTick;
	//Task8
	task8.state = start;
	task8.period = 200;
	task8.elapsedTime = task8.period;
	task8.TickFct = &Festive2DefaultSMTick;
	
	TimerSet(200);
	TimerOn();
	
	unsigned short i; //scheduler loop iterator
	while(1) {
		for (i = 0; i < numTasks; i++) {
			tmpA = ~PINA;
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
