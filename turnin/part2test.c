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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "scheduler.h"
#endif

void transmit_data(unsigned char data) {

    int i;

    for (i = 0; i < 8 ; ++i) {

   	 // Sets SRCLR to 1 allowing data to be set

   	 // Also clears SRCLK in preparation of sending data

   	 PORTC = 0x08;

   	 // set SER = next bit of data to be sent.

   	 PORTC |= ((data >> i) & 0x01);

   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register

   	 PORTC |= 0x02;

    }

    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register

    PORTC |= 0x04;

    // clears all lines in preparation of a new transmission

    PORTC = 0x00;

}

//shared variables
unsigned char go;
unsigned char button;

enum down_States{down_wait, down_display1, down_display2, down_display3};

int down_Tick(int state){
	switch(state){
	case down_wait:
		if(go == 0x02){
			state = down_display1;
		}
		break;
	case down_display1:
		if(go == 0x02){
		state = down_display2;
		}
		else{
			state = down_wait;
		}
		break;
	case down_display2:
		if(go == 0x02){
		state = down_display3;
		}
		else {
			state = down_wait;
		}
		break;
	case down_display3:
		if(go== 0x02){
		state = down_display1;
		}
		else{
			state = down_wait;
		}
		break;
	default:
		state = down_display1;
		break;
	}
	switch(state){
		case down_wait:
			break;
		case down_display1:
			transmit_data(0xc3);
			break;
		case down_display2:
			transmit_data(0x18);
			break;
		case down_display3:
			transmit_data(0x24);
			break;
		default:
			break;
	}
	return state;
}

enum default_States{default_wait, default_display1, default_display2, default_display3, default_display4};

int default_Tick(int state){
	switch(state){
		case default_wait:
			if(go == 0x00){
				state = default_display1;
			}
			break;
		case default_display1:
			if(go == 0x00){
			state = default_display2;
			}
			else{
				state = default_wait;
			}
			break;
		case default_display2:
			if(go ==0x00){
			state = default_display3;
			}
			else{
				state = default_wait;
			}
			break;
		case default_display3:
			if(go==0x00){
			state = default_display4;
			}
			else{
				state = default_wait;
			}
			break;
		case default_display4:
			if(go == 0x00){
			state = default_display1;
			}
			else{
				state = default_wait;
			}
			break;
		default:
			state = default_display1;
			break;
		}
	switch(state){
		case default_wait:
			break;
		case default_display1:
			transmit_data(0xAA);
			break;
		case default_display2:
			transmit_data(0x55);
			break;
		case default_display3:
			transmit_data(0x3C);
			break;
		case default_display4:
			transmit_data(0xA5);
			break;
		default:
			break;
	}
	return state;
}

enum up_States{up_wait, up_display1, up_display2, up_display3};

int up_Tick(int state){
	switch(state){
		case up_wait:
			if(go == 0x01){
				state = up_display1;
			}
			break;
		case up_display1:
			if(go == 0x01){
			state = up_display2;
			}
			else{
				state = up_wait;
			}
			break;
		case up_display2:
			if(go == 0x01){
			state = up_display3;
			}
			else{
				state = up_wait;
			}
			break;
		case up_display3:
			if(go == 0x01){
			state = up_display1;
			}
			else{
				state = up_wait;
			}
			break;
		default:
			state = up_display1;
			break;
		}
	switch(state){
		case up_wait:
			break;
		case up_display1:
			transmit_data(0x18);
			break;
		case up_display2:
			transmit_data(0x66);
			break;
		case up_display3:
			transmit_data(0xE7);
			break;
		default:
			break;
	}
	return state;
}

enum which_SM_States{ down_, default_, up_, off, off_wait};

int which_SM_Tick(int state){
	switch(state){
		case default_:
			if(button == 0x01){
				state = up_;
			}
			else if(button == 0x02){
				state = down_;
			}
			else if(button == 0x03){
				state = off;
			}
			break;
		case up_:
			if(button == 0x02){
				state = default_;
			}
			else if(button == 0x03){
				state = off;
			}
			break;
		case down_:
			if(button == 0x01){
				state = default_;
			}
			else if(button == 0x03){
				state = off;
			}
			break;
		case off:
			if(button == 0x00){
			state = off_wait;
			}
			break;
		case off_wait:
			if(button == 0x03){
				state = default_;
			}
			break;
		default:
			state = default_;
			break;
		}
	switch(state){
		case default_:
			go = 0x00;
			break;
		case up_:
			go = 0x01;
			break;
		case down_:
			go = 0x02;
			break;
		case off:
			go = 0x03;
			transmit_data(0x00);
			break;
		case off_wait:
			break;
		}
	return state;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xff;
	DDRB = 0xff; PORTB = 0x00;
	DDRC = 0xff; PORTC = 0x00;
    /* Insert your solution below */
	button = 0x00;

	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 200;
	task1.elapsedTime = task1.period;
	task1.TickFct =&down_Tick;

	task2.state = start;
	task2.period = 200;
	task2.elapsedTime = task2.period;
	task2.TickFct = &default_Tick;

	task3.state= start;
	task3.period= 200;
	task3.elapsedTime = task3.period;
	task3.TickFct = &up_Tick;

	task4.state = start;
	task4.period = 200;
	task4.elapsedTime = task4.period;
	task4.TickFct= which_SM_Tick;

	unsigned long int findGCD(unsigned long int a, unsigned long int b){
		unsigned long int c;
		while(1){
			c = a%b;
			if (c==0){
				return b;
			}
			a = b;
			b = c;
		}
		return 0;
	}
	unsigned short i;
	unsigned long GCD = tasks[0]->period;
	for(i=1; i< numTasks; i++){
		GCD = findGCD(GCD, tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();


    /* Insert your solution below */
    while (1) {
	button = ~PINA;
	for(i=0; i<numTasks; i++){
		if(tasks[i]->elapsedTime == tasks[i]->period){
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += GCD;
	}
	while(!TimerFlag){};
	TimerFlag = 0;
    }

    return 1;
}
