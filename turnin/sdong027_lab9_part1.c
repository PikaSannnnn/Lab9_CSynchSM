/*	Author: sdong027
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
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

enum ThreeLEDs {ONE, TWO, THREE} LED3_STATES = -1;	// initialize to run default case (start/rst case)
unsigned char ThreeLEDsSM() {
	unsigned char threeLEDs = 0x00;
	switch(LED3_STATES) {
		case ONE:
			LED3_STATES = TWO;
			break;
		case TWO:
			LED3_STATES = THREE;
			break;
		case THREE:
			LED3_STATES = ONE;
			break;
		default:
			threeLEDs = 0x00;
			LED3_STATES = ONE;
			break;
	}

	switch(LED3_STATES) {
		case ONE:
			threeLEDs = (threeLEDs & 0x00) | 0x01;
			break;
		case TWO:
			threeLEDs = (threeLEDs & 0x00) | 0x02;
			break;
		case THREE:
			threeLEDs = (threeLEDs & 0x00) | 0x04;
			break;
	}

	return threeLEDs;
}

enum BlinkingLEDs {OFF, ON} BLINK_STATES = -1;	// initialize to run default case (start/rst case)
unsigned char BlinkingLEDSM() {
	unsigned char blinkingLED = 0x00;
	switch(BLINK_STATES) {
		case OFF:
			BLINK_STATES = ON;
			break;
		case ON:
			BLINK_STATES = OFF;
			break;
		default:
			blinkingLED = 0x00;
			BLINK_STATES = OFF;
			break;
	}

	switch(BLINK_STATES) {
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
	TimerSet(1000);
	TimerOn();

	DDRB = 0xFF; PORTB = 0x00;
	unsigned char threeLEDs = 0x00;
	unsigned char blinkLED = 0x00;

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
