/*
 * events.h
 *
 * Created: 30.10.2022 12:26:42
 *  Author: werbu
 */ 

#ifndef EVENTS_H_
#define EVENTS_H_

#include <stdint.h>

typedef uint8_t event_type;

void setEvent(event_type ev);

void clearEvent(event_type ev);

int eventIsSet(event_type ev);

#endif /* EVENTS_H_ */