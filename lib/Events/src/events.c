/*
 * events.c
 *
 * Created: 11.01.2025 14:31:47
 *  Author: werbu
 */ 
#include "events.h"
#include <avr/interrupt.h> 

volatile event_type events=0;

void setEvent(event_type ev) {
	uint8_t sreg_old = SREG;
	cli();
	events |= ev;
	SREG = sreg_old;
}

void clearEvent(event_type ev) {
	unsigned char sreg_old = SREG;
	cli();
	events &= ~ev;
	SREG = sreg_old;
}

int eventIsSet(event_type ev){
	return (events&ev);
}
