#include "joystick.h"
#include "IO_Macros.h"

void joystick_init(void){
    ADMUX = (1 << REFS0); //use Vcc (5,0V)
    ADCSRA = (1 << ADEN); // turn on ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 
    
    PinMode(JOYSTICK_B_PIN, Input);
    DigitalWrite(JOYSTICK_B_PIN, High); // pull up

}

uint16_t ADC_read(uint8_t channel){
    //clear old values and set new
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // start conversation
    ADCSRA |= (1 << ADSC);

    // wait until completed
    while (ADCSRA & (1 << ADSC));

    return ADC;
}

joystick_data joystick_read(void){
    joystick_data data;
    data.x = ADC_read(1);
    data.y = ADC_read(0);

    if (DigitalRead(JOYSTICK_B_PIN))
        data.is_pressed = true;
    else data.is_pressed = false;

    return data;
}
