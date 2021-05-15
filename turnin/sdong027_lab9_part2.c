/*	Author: sdong027
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *	Link: https://www.youtube.com/watch?v=ZTvqTOd8jFE
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
void CombineLEDsSM(unsigned char led3, unsigned char blinkLED) {
	PORTB = 0x00 | (led3 | blinkLED);	
} 

int main(void) {
	/* Insert DDR and PORT initializations */
	TimerSet(1);	// (Do not tick the timer at the GCD of the tasks)
	TimerOn();

	DDRB = 0xFF; PORTB = 0x00;
	unsigned char threeLEDs = 0x00;
	unsigned char blinkLED = 0x00;
	LED3_STATES = LEDSTART;
	BLINK_STATES = BLINKSTART;

	/* Insert your solution below */
	while (1) {
		threeLEDs = ThreeLEDsSM();
		blinkLED = BlinkingLEDSM();
		CombineLEDsSM(threeLEDs, blinkLED);
		while (!TimerFlag);
		TimerFlag = 0;
	}
	return 1;
}
