#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED
/*
||
||  Filename:	 		DHT.h
||  Title: 			    DHTxx Driver
||  Author: 			Kyrylo Klitsman
||	Email:				
||  Compiler:		 	AVR-GCC
||	Description:
||	This library can drive joystick HW-504
||
*/
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#define JOYSTICK_X_PIN C, 1
#define JOYSTICK_Y_PIN C, 0
#define JOYSTICK_B_PIN B, 2

typedef struct {
    uint16_t x;
    uint16_t y;
    bool is_pressed;
} joystick_data;

void joystick_init(void);
joystick_data joystick_read(void);

#endif