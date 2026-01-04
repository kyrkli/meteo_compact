#ifndef LED_RGB_H_INCLUDED
#define LED_RGB_H_INCLUDED
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


#define LED_BLUE_PIN D, 6
#define LED_GREEN_PIN D, 7
#define LED_RED_PIN B, 0

typedef enum{
	HUM_NOT_INIT,
	HUM_ERROR,
	HUM_VERY_DRY,
	HUM_DRY,
	HUM_COMFORT,
	HUM_HUMID,
	HUM_VERY_HUMID
} humidity_level;

humidity_level get_humidity_level(double hum);

void rgb_hum_level(humidity_level hum_level);


#endif