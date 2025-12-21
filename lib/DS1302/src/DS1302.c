/*
 * DS1302.c
 *
 * Created: 19.06.2025 22:59:38
 *  Author: werbu
 */ 
/** Ds1302.cpp
 *
 * Ds1302 class.
 *
 * @version 1.0.3
 * @author Rafa Couto <caligari@treboada.net>
 * @license GNU Affero General Public License v3.0
 * @see https://github.com/Treboada/Ds1302
 *
 */

#include "DS1302.h"
#include "IO_Macros.h"
#include <util/delay.h>

#define REG_SECONDS           0x80
#define REG_MINUTES           0x82
#define REG_HOUR              0x84
#define REG_DATE              0x86
#define REG_MONTH             0x88
#define REG_DAY               0x8A
#define REG_YEAR              0x8C
#define REG_WP                0x8E
#define REG_BURST             0xBE

static int _dat_direction;

void DS1302_init()
{
    _dat_direction = Input;

    PinMode(DS1302_PIN_ENA, Output);
    PinMode(DS1302_PIN_CLK, Output);
    PinMode(DS1302_PIN_DAT, _dat_direction);
	
    DigitalWrite(DS1302_PIN_ENA, Low);
    DigitalWrite(DS1302_PIN_CLK, Low);
}

bool DS1302_isHalted()
{
    _prepareRead(REG_SECONDS);
    uint8_t seconds = _readByte();
    _end();
    return (seconds & 0b10000000); 
}

void DS1302_getDateTime(DateTime* dt)
{
    _prepareRead(REG_BURST);
    dt->second = _bcd2dec(_readByte() & 0b01111111);
    dt->minute = _bcd2dec(_readByte() & 0b01111111);
    dt->hour   = _bcd2dec(_readByte() & 0b00111111);
    dt->day    = _bcd2dec(_readByte() & 0b00111111);
    dt->month  = _bcd2dec(_readByte() & 0b00011111);
    dt->dow    = _bcd2dec(_readByte() & 0b00000111);
    dt->year   = _bcd2dec(_readByte() & 0b11111111);
    _end();
}

void DS1302_setDateTime(DateTime* dt)
{
    // turn off write protection
    _prepareWrite(REG_WP);
    _writeByte(0b00000000);
    _end();

    // burst write
    _prepareWrite(REG_BURST);
    _writeByte(_dec2bcd(dt->second % 60));
    _writeByte(_dec2bcd(dt->minute % 60));
    _writeByte(_dec2bcd(dt->hour   % 24));
    _writeByte(_dec2bcd(dt->day    % 32));
    _writeByte(_dec2bcd(dt->month  % 13));
    _writeByte(_dec2bcd(dt->dow    % 8));
    _writeByte(_dec2bcd(dt->year   % 100));
    _writeByte(0b10000000);
    _end();
}

void halt()
{
    _setHaltFlag(true);
}

void start()
{
    _setHaltFlag(false);
}

void _setHaltFlag(bool stopped)
{
    _prepareRead(REG_SECONDS);
    uint8_t seconds = _readByte();
    _end();

    if (stopped) 
        seconds |= 0b10000000; 
    else seconds &= ~0b10000000;

    _prepareWrite(REG_WP);
    _writeByte(0x00);
    _end();

    _prepareWrite(REG_SECONDS);
    _writeByte(seconds);
    _end();

    _prepareWrite(REG_WP);
    _writeByte(0b10000000);
    _end();

}

void _setDirection(int direction)
{
    if (_dat_direction != direction)
    {
        _dat_direction = direction;
        PinMode(DS1302_PIN_DAT, direction);
    }
}

void _prepareRead(uint8_t address)
{
    _setDirection(Output);
    DigitalWrite(DS1302_PIN_ENA, High);
    uint8_t command = 0b10000001 | address;
    _writeByte(command);
    _setDirection(Input);
}

void _prepareWrite(uint8_t address)
{
    _setDirection(Output);
    DigitalWrite(DS1302_PIN_ENA, High);
    uint8_t command = 0b10000000 | address;
    _writeByte(command);
}

void _end()
{
    DigitalWrite(DS1302_PIN_ENA, Low);
    _delay_us(5);
}

void _nextBit()
{   
    _delay_us(1);
    DigitalWrite(DS1302_PIN_CLK, High);
    _delay_us(1);
    DigitalWrite(DS1302_PIN_CLK, Low);
    _delay_us(1);
}


void _writeByte(uint8_t value)
{
    _setDirection(Output);
    for(uint8_t b = 0; b < 8; b++)
    {
        DigitalWrite(DS1302_PIN_DAT, ((value & 0x01) ? High : Low));
        
        _nextBit();
        
        value >>= 1;
    }
}

uint8_t _readByte()
{
    uint8_t byte = 0;
    _setDirection(Input);

    for(uint8_t b = 0; b < 8; b++)
    {
        if (DigitalRead(DS1302_PIN_DAT) == High) 
            byte |= (0x01 << b);
        _nextBit();
    }

    return byte;
}

uint8_t _dec2bcd(uint8_t dec)
{
    return ((dec / 10 * 16) + (dec % 10));
}

uint8_t _bcd2dec(uint8_t bcd)
{
    return ((bcd / 16 * 10) + (bcd % 16));
}