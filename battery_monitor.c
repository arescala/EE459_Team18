#include "battery_monitor.h"

void init_battery_monitor()
{
    ADMUX |= (REFS0);  //set ADC reference to AVCC
    ADMUX |= (1 << ADLAR); // 8-bit conversions, 8 is enough

    // 7372800/128 = 57.6 kHz, good for ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescalar = 128
    ADCSRA |= (1 << ADEN); // enable ADC
}

uint8_t get_battery_voltage()
{
    uint8_t result = 0;
    ADMUX &= 0xf0;  //clear previous selection
    ADMUX |= BATTERY_VOLTAGE_CHANNEL;

    //get sample
    ADCSRA |= (1 << ADSC);
    while ( ADCSRA & (1 << ADSC) );
    result = ADCH;
    return result;
}
