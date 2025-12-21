/*
 * usart.c
 *
 * Created: 11.01.2025 11:18:15
 *  Author: werbu
 */ 

#include "usart.h"
#include <avr/io.h>

unsigned char buffer[BUFFERSIZE];
uint8_t read=0;
uint8_t write=0;
uint8_t freeSpace = BUFFERSIZE;

uint8_t txOn = 1;
uint8_t rx_allowed = 1;

void USART_Init(){
	UBRR0H = (BAUD_CONST >> 8);
	UBRR0L = BAUD_CONST;
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
}

void USART_Transmit(unsigned char data){
	while(!rx_allowed) ;
	while(!(UCSR0A & (1<<UDRE0))) ;
	UDR0 = data;
}

unsigned char USART_Receive(){
	if(freeSpace < BUFFERSIZE){
		uint8_t data = buffer[read];
		read++;
		if(read == BUFFERSIZE)
		read=0;
		freeSpace++;
		if(txOn == 0 && freeSpace >= BUFFERSIZE-10){
			txOn = 1;
			USART_Transmit(xOn);
		}
		return data;
	}
	return 0;
}

void USART_Transmit_string(const char* string){
	for(int i = 0; string[i] != '\0'; i++)
	USART_Transmit(string[i]);
}

void USART_Transmit_double(double d){
	char buf[8];
	dtostrf(d, 1, 1, buf);
	USART_Transmit_string(buf);
}

ISR(USART0_RX_vect){
	unsigned char data = UDR0;
	if(data == xOff){
		rx_allowed = 0;
	}
	else if(data == xOn){
		rx_allowed = 1;
	}
	else{
		freeSpace--;
		if(freeSpace <= 5 && txOn == 1){
			USART_Transmit(xOff);
			txOn = 0;
		}
		buffer[write] = data;
		write++;
		if(write == BUFFERSIZE)
		write = 0;
	}

}