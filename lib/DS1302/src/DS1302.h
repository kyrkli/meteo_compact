/*
 * DS1302.h
 *
 * Created: 19.06.2025 22:52:28
 *  Author: werbu
 */ 
/** Ds1302.h
 *
 * Ds1302 class.
 *
 * @version 1.0.3
 * @author Rafa Couto <caligari@treboada.net>
 * @license GNU Affero General Public License v3.0
 * @see https://github.com/Treboada/Ds1302
 *
 */

#ifndef DS1302_H_
#define DS1302_H_


#include <stdint.h>
#include <stdbool.h>

#define DS1302_PIN_ENA D, 2
#define DS1302_PIN_DAT D, 3
#define DS1302_PIN_CLK D, 4

typedef struct {
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
    uint8_t second;
    uint8_t dow;
} DateTime;

/**
* Months of year
*/
enum MONTH {
    MONTH_JAN = 1,
    MONTH_FEB = 2,
    MONTH_MAR = 3,
    MONTH_APR = 4,
    MONTH_MAY = 5,
    MONTH_JUN = 6,
    MONTH_JUL = 7,
    MONTH_AUG = 8,
    MONTH_SEP = 9,
    MONTH_OCT = 10,
    MONTH_NOV = 11,
    MONTH_DEC = 12
};

/**
* Days of week
*/
enum DOW {
    DOW_MON = 1,
    DOW_TUE = 2,
    DOW_WED = 3,
    DOW_THU = 4,
    DOW_FRI = 5,
    DOW_SAT = 6,
    DOW_SUN = 7
};

/**
* Initializes the DW1302 chip.
*/
void DS1302_init();

/**
* Returns when the oscillator is disabled.
*/
bool DS1302_isHalted();

/**
* Stops the oscillator.
*/
void halt();

/**
* Starts the oscillator.
*/
void start();

/**
* Returns the current date and time.
*/
void DS1302_getDateTime(DateTime* dt);

/**
* Sets the current date and time.
*/
void DS1302_setDateTime(DateTime* dt);

//private:

uint8_t _pin_ena;
uint8_t _pin_clk;
uint8_t _pin_dat;

void _prepareRead(uint8_t address);
void _prepareWrite(uint8_t address);
void _end();

//int _dat_direction;
void _setDirection(int direction);

uint8_t _readByte();
void _writeByte(uint8_t value);
void _nextBit();

uint8_t _dec2bcd(uint8_t dec);
uint8_t _bcd2dec(uint8_t bcd);

void _setHaltFlag(bool stopped);


#endif /* DS1302_H_ */