/*	Author: sdong027
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

const int led3Ticks = (300);
const int blinkTicks = (1000);
int spkrTicks = 1;

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff() {
	TCCR1B = 0x00;
}
void TimerISR() {
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

enum ThreeLEDs {LEDSTART, ONE, TWO, THREE} LED3_STATES;
unsigned char ThreeLEDsSM() {
	unsigned char threeLEDs;
	static int i;
	switch(LED3_STATES) {
		case LEDSTART:
			LED3_STATES = ONE;
			i = 0;
			break;
		case ONE:
			i++;
			if (i >= led3Ticks) {
				LED3_STATES = TWO;
				i = 0;
			}
			break;
		case TWO:
			i++;
			if (i >= led3Ticks) {
				LED3_STATES = THREE;
				i = 0;
			}
			break;
		case THREE:
			i++;
			if (i >= led3Ticks) {
				LED3_STATES = ONE;
				i = 0;
			}
			break;
		default:
			threeLEDs = 0x00;
			i = 0;
			LED3_STATES = ONE;
			break;
	}

	switch(LED3_STATES) {
		case LEDSTART:
			// nothing
			break;
		case ONE:
			threeLEDs = 0x01;
			break;
		case TWO:
			threeLEDs = 0x02;
			break;
		case THREE:
			threeLEDs = 0x04;
			break;
	}

	return threeLEDs;
}

enum BlinkingLEDs {BLINKSTART, OFF, ON} BLINK_STATES;
unsigned char BlinkingLEDSM() {
	unsigned char blinkingLED;
	static int i;
	switch(BLINK_STATES) {
		case BLINKSTART:
			BLINK_STATES = OFF;
			i = 0;
			break;
		case OFF:
			i++;
			if (i >= blinkTicks) {
				BLINK_STATES = ON;
				i = 0;
			}
			break;
		case ON:
			i++;
			if (i >= blinkTicks) {
				BLINK_STATES = OFF;
				i = 0;
			}
			break;
		default:
			blinkingLED = 0x00;
			BLINK_STATES = OFF;
			i = 0;
			break;
	}

	switch(BLINK_STATES) {
		case BLINKSTART:
			// nothing
			break;
		case OFF:
			blinkingLED = 0x00;
			break;
		case ON:
			blinkingLED = 0x08;
			break;
	}

	return blinkingLED;
}

enum Speaker {SPKRSTART, STOP, PLAY} SPKR_STATES;
unsigned char SpkrSM() {
	unsigned char spkr;
	static int i;
	switch(SPKR_STATES) {
		case SPKRSTART:
			SPKR_STATES = PLAY;
			i = 0;
			break;
		case STOP:
			i++;
			if (i >= spkrTicks) {
				SPKR_STATES = PLAY;
				i = 0;
			}
			break;
		case PLAY:
			i++;
			if (i >= spkrTicks) {
				SPKR_STATES = STOP;
				i = 0;
			}
			break;
		default:
			spkr = 0x00;
			i = 0;
			SPKR_STATES = STOP;
			break;
	}

	switch(SPKR_STATES) {
		case SPKRSTART:
			// nothing
			break;
		case STOP:
			spkr = 0x10;
			break;
		case PLAY:
			spkr = 0x00;
			break;
	}

	return spkr;
}

enum Freq {WAIT, INC, WAIT_INC, DEC, WAIT_DEC} FREQ_STATES;	// INC = increase freq, DEC = decrease freq
void ChangeFreqSM(const unsigned char input) {
	switch(FREQ_STATES) {
		case WAIT:
			if (input & 0x01) {	// up
				FREQ_STATES = INC;
			}
			else if (input & 0x02) {	// down
				FREQ_STATES = DEC;
			}
			break;
		case INC:
			FREQ_STATES = WAIT_INC;
			break;
		case WAIT_INC:
			if (!(input & 0x01) && (input & 0x02)) {	// up freq
				FREQ_STATES = DEC;
			}
			else if (!input) {	// no press
				FREQ_STATES = WAIT;
			}
			break;
		case DEC:
			FREQ_STATES = WAIT_DEC;
			break;
		case WAIT_DEC:
			if ((input & 0x01) && !(input & 0x02)) {	// down freq
				FREQ_STATES = INC;
			}
			else if (!input) {	// no press
				FREQ_STATES = WAIT;
			}
			break;
	}

	switch(FREQ_STATES) {
		case WAIT:
			// nothing
			break;
		case INC:
			if (spkrTicks > 1) {
				spkrTicks--;
			}
			break;
		case WAIT_INC:
			// nothing
			break;
		case DEC:
			spkrTicks++;
			break;
		case WAIT_DEC:
			// nothing
			break;
	}
}
void CombineLEDsSM(unsigned char input, unsigned char led3, unsigned char blinkLED, unsigned char spkr) {
	unsigned char tmpB = 0x00 | (led3 | blinkLED);
	if (input >> 2) {
		tmpB = (tmpB | spkr);
	}

	PORTB = tmpB;		
} 

int main(void) {
	/* Insert DDR and PORT initializations */
	TimerSet(1);	// (Do not tick the timer at the GCD of the tasks)
	TimerOn();

	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	unsigned char input = 0x00;
	unsigned char threeLEDs = 0x00;
	unsigned char blinkLED = 0x00;
	unsigned char spkr = 0x00;

	LED3_STATES = LEDSTART;
	BLINK_STATES = BLINKSTART;
	SPKR_STATES = SPKRSTART;
	FREQ_STATES = WAIT;

	/* Insert your solution below */
	while (1) {
		input = ~PINA & 0x07;	// gets A0 - A2
		threeLEDs = ThreeLEDsSM();
		blinkLED = BlinkingLEDSM();
		spkr = SpkrSM();

		ChangeFreqSM(input);
		CombineLEDsSM(input, threeLEDs, blinkLED, spkr);

		while (!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
