/*
 * status_display.c
 *
 * Created: 04.05.2025 21:37:20
 * Author : werbu
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "DHT.h"
#include "usart.h"
#include "lcd.h"
#include "DS1302.h"
#include "timer.h"
#include "joystick.h"
#include "led_rgb.h"
#include "screen.h"
#include "events.h"

void setup_DS1302()
{
	if (DS1302_isHalted())
	{
		DateTime dt;
		dt.year = 25;
		dt.month = MONTH_DEC;
		dt.day = 13;
		dt.hour = 11;
		dt.minute = 26;
		dt.second = 00;
		dt.dow = DOW_SAT;

		DS1302_setDateTime(&dt);

		start();
	}
	
	_delay_ms(100);
}

#define TIMER_START_DHT11 0
#define TIMER_IDLE_SCR 1

#define EV_START_DHT11 1
#define EV_IDLE_SCREEN 2

void cb_start_dht11(){
	setEvent(EV_START_DHT11);
}

void cb_idle_screen(){
	setEvent(EV_IDLE_SCREEN);
}

/*
	Measurement Range
	DHT11 20-90%RH
	0-50 ℃
*/

// 336*2 bytes for temp and humidity
// 336 bytes for one type

#define MAX_SAVED_TMP_HUM 336

uint16_t write_index = 0;
bool buffer_full = false;

void save_measurement(uint8_t *saved_tmp, uint8_t *saved_hum, uint8_t tmp, uint8_t hum) {
    saved_tmp[write_index] = tmp;
    saved_hum[write_index] = hum;

    if (++write_index == MAX_SAVED_TMP_HUM) {
        write_index = 0;
		buffer_full = true;
    }
}

int map_temp_to_y(temp_val){
	// from 0 to 50 -> 0 to 64

}

#define __DHT_Temp_Min	0
#define __DHT_Temp_Max	50

#define mapRange(a1,a2,b1,b2,s) (b1 + (s-a1)*(b2-b1)/(a2-a1))

void draw_graph(uint8_t *saved_tmp) {
    int x_on_screen = 0;
    
    uint8_t start_reading_from = buffer_full ? write_index : 0;

	// for current day only 48 points twice pro hour
	uint8_t points_to_draw = (write_index > 48) ? 48 : write_index;
	
    for (int i = 0; i < points_to_draw; i++) {
        uint8_t temp_val = saved_tmp[start_reading_from + i];
        
        // Рисуем пиксель
        // x_on_screen — координата X на экране
        // temp_val — нужно преобразовать в координату Y
        lcd_drawPixel(x_on_screen, mapRange(__DHT_Temp_Min, __DHT_Temp_Max, 0, 63, temp_val), WHITE);
        
        x_on_screen += 3;
    }
}

int main(void)
{
	//Variables
	double temperature;
	double humidity;
	char buf[16];
	DateTime current_datatime;

	// for 1 week twice pro hour
	uint8_t saved_tmp[MAX_SAVED_TMP_HUM];
	uint8_t saved_hum[MAX_SAVED_TMP_HUM];
	uint8_t counter_10sec = 0;
	uint8_t counter_minutes = 0;


	declareTimer(TIMER_START_DHT11, 10, cb_start_dht11);
	declareTimer(TIMER_IDLE_SCR, 60, cb_idle_screen);


	// Initialization
	DHT_Setup();
	lcd_init(LCD_DISP_ON);
	USART_Init();
	DS1302_init();
	joystick_init();
	
	sei();

	startTimer(TIMER_START_DHT11);
	startTimer(TIMER_IDLE_SCR);

	PinMode(LED_BLUE_PIN, Output);
	PinMode(LED_GREEN_PIN, Output);
	PinMode(LED_RED_PIN, Output);

	humidity_level prev_hum_level = HUM_NOT_INIT;
	humidity_level curr_hum_level;
	//setup_DS1302();
    /* Replace with your application code */

	SCREEN_STATE current_screen = SCREEN_INFO;
	joystick_data jdata;
	bool is_screen_updated = false;

	DHT_Read(&temperature, &humidity);
	
	uint8_t y_pixel = 0;

    while (1) 
    {
		if(eventIsSet(EV_START_DHT11)){
			clearEvent(EV_START_DHT11);
			DHT_Read(&temperature, &humidity);
			counter_10sec++;

			if(counter_10sec == 6){ //180
				counter_10sec = 0;
				save_measurement(saved_tmp, saved_hum, (uint8_t) temperature, (uint8_t) humidity);
			}
		}

		if(eventIsSet(EV_IDLE_SCREEN)){
			clearEvent(EV_IDLE_SCREEN);
			current_screen = SCREEN_IDLE;
		}

		jdata = joystick_read();

		if(jdata.x > 900){
			current_screen++;
			if(current_screen == (SCREEN_GRAPHS + 1)) current_screen = 0;
			is_screen_updated = true;
		}
		else if (jdata.x < 100) {

            if (current_screen == 0) current_screen = SCREEN_GRAPHS;
            else current_screen--;
            is_screen_updated = true;
        }

		if(is_screen_updated){
			lcd_clrscr();
			resetTimer(TIMER_IDLE_SCR);
			is_screen_updated = false;
		}
		
		lcd_clear_buffer();

		switch (current_screen)
		{
		case SCREEN_INFO:
			
			if(DHT_GetStatus() == DHT_Ok){
				dtostrf(temperature, 1, 1, buf);
				
				lcd_gotoxy(3,0);
				lcd_puts(buf);
				lcd_puts(" °C");
				
				dtostrf(humidity, 1, 1, buf);
				lcd_gotoxy(3,2);
				lcd_puts(buf);
				lcd_puts(" %RH");
				
				curr_hum_level = get_humidity_level(humidity);
				if(curr_hum_level != prev_hum_level)
					rgb_hum_level(curr_hum_level);
				prev_hum_level = curr_hum_level; 
			}
			else {
				sprintf(buf, "DHT Error");
				lcd_gotoxy(3,0);
				lcd_puts(buf);

				curr_hum_level = HUM_ERROR;
				if(curr_hum_level != prev_hum_level)
					rgb_hum_level(curr_hum_level);
				prev_hum_level = curr_hum_level; 
			}

			DS1302_getDateTime(&current_datatime);
			//print current time
			sprintf(buf, "%02u:%02u:%02u", current_datatime.hour, current_datatime.minute, current_datatime.second);
			lcd_gotoxy(3,4);
			lcd_puts(buf);
			break;
		case SCREEN_SYSTEM:
			
			sprintf(buf, "System Uptime:");
			lcd_gotoxy(0,0);
			lcd_puts(buf);
			
			uint8_t y = 1;
			uint32_t current_time = get_current_time();
			
			if(current_time / 3600 > 0){
				sprintf(buf, "Hours: %d", (current_time / 360));
				lcd_gotoxy(0,y++);
				lcd_puts(buf);
			}

			if(current_time / 60 > 0){
				sprintf(buf, "Minutes: %d", (current_time / 60));
				lcd_gotoxy(0,y++);
				lcd_puts(buf);
			}

			sprintf(buf, "Seconds: %d", (current_time % 60));
			lcd_gotoxy(0,y++);
			lcd_puts(buf);

			break;
		case SCREEN_IDLE:

			if(temperature < 19){
				(y_pixel % 2) ? lcd_drawBitmap(0, y_pixel+6, cold_bitmap, 50, 50, 1) :
								lcd_drawBitmap(0, y_pixel+6, cold_bitmap2, 50, 50, 1);
				(y_pixel % 2) ? lcd_drawBitmap(42, y_pixel, cold_bitmap, 50, 50, 1) :
								lcd_drawBitmap(42, y_pixel, cold_bitmap2, 50, 50, 1);
				(y_pixel % 2) ? lcd_drawBitmap(84, y_pixel+6, cold_bitmap, 50, 50, 1) :
								lcd_drawBitmap(84, y_pixel+6, cold_bitmap2, 50, 50, 1);
			} else if(temperature >= 19 && temperature <= 24){
				lcd_drawBitmap(8, 0, cat_bitmap, 128, 64, 1);
				(y_pixel % 2) ? lcd_drawBitmap(72, 0, sun_bitmap, 50, 50, 1) :
								lcd_drawBitmap(72, 0, sun2_bitmap, 50, 50, 1);
			} else { //TODO Hot image

			}

			if(y_pixel >= 40)
				y_pixel = 0;
			else y_pixel += 3;

			break;
		case SCREEN_GRAPHS:
			draw_graph(saved_hum);
			break;
		default:
			break;
		}
		lcd_display();

		_delay_ms(500);
    }
}

