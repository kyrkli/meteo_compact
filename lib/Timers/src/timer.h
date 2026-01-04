/*
 * timer.h
 * use sei()
 * Created: 29.10.2022 10:05:54
 *  Author: kyrylo
 */ 

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#define MAX_TIMER_NUM 5

typedef enum {
	s = 0x04,
	ms = 0x02
} TimeGranularity;

struct Timer{
	uint8_t flags; //1 bit - active/inactive
	uint32_t duration;
	uint32_t start_time;
	void (*cb)();
};

uint32_t getDuration(uint8_t N);

void declareTimer(uint8_t N, uint32_t _duration, void (*_cb)());

void startTimer(uint8_t N);

void resetTimer(uint8_t N);

void stopTimer(uint8_t N);

uint32_t actualTime(uint8_t N);

void checkTimer();

uint32_t get_current_time();

#endif /* TIMER_H_ */