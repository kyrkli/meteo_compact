/*
 * usart.h
 *
 * Created: 10-Jan-23 22:01:59
 *  Author: werbu
 */ 


#ifndef USART_H_
#define USART_H_

#define BUFFERSIZE 32
#define BAUDRATE 9600
#define BAUD_CONST (((F_CPU/(BAUDRATE*16UL)))-1)

#define xOff 0x13
#define xOn 0x11

void USART_Init();

void USART_Transmit(unsigned char data);

unsigned char USART_Receive();

void USART_Transmit_string(const char* string);

void USART_Transmit_double(double d);

#endif /* USART_H_ */