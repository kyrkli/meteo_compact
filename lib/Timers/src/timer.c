/*
 * timer.c
 *
 * Created: 11.01.2025 10:58:49
 *  Author: werbu
 */ 
#include "timer.h"

volatile uint32_t current_time = 0;
uint16_t count_from = 0;

struct Timer timer[MAX_TIMER_NUM];

TimeGranularity timer_mode = s;

uint32_t getDuration(uint8_t N){
	return timer[N].duration;
}

void declareTimer(uint8_t N, uint32_t _duration, void (*_cb)()){
	timer[N].flags = 0;
	timer[N].duration = _duration;
	timer[N].start_time = 0;
	timer[N].cb = _cb;
	
	//settings for TIMER1
	TCCR1A = 0;
	
	//Interrupt for Timer1
	TIMSK1 |= (1<<TOIE1);
	
	//clear prescaler bits
	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
	

	if (timer_mode == s){
		TCCR1B |= (1<<CS12)|(1<<CS10);	//Prescaler to 1024
		count_from = 49911; 			//counts from 49911 to 65536 = 15625 --> every s triggers an interrupt
		TCNT1 = count_from;
	}
	else if (timer_mode == ms){
		TCCR1B |= (1<<CS11)|(1<<CS10);	//Prescaler to 64
		count_from = 65286;				//counts from 65286 to 65536 = 250 --> every ms triggers an interrupt
		TCNT1 = count_from;
	}	
}

void startTimer(uint8_t N){
	timer[N].flags |= 1;
	timer[N].start_time = current_time;
}

void resetTimer(uint8_t N){
	timer[N].start_time = current_time;
}

void stopTimer(uint8_t N){
	timer[N].flags &= ~1;
	timer[N].start_time = 0;
}

uint32_t actualTime(uint8_t N){
	if(timer[N].flags&0x01)
		return current_time - timer[N].start_time;
}

void checkTimer(){
	for(int i=0; i< MAX_TIMER_NUM; i++)
		if(timer[i].flags&0x01)
			if(current_time - timer[i].start_time >= timer[i].duration){
				timer[i].start_time = current_time;
				timer[i].cb();
			}
}

uint32_t get_current_time(){
	return current_time;
}

ISR(TIMER1_OVF_vect){
	current_time += 1;
	TCNT1 = count_from;
	checkTimer();
}