#include "battery_monitor.h"

uint8_t get_battery_voltage()
{
    uint8_t result = 0;
    ADMUX &= 0xf0;  //clear previous selection
    ADMUX |= channel;

    //get sample
    ADCSRA |= (1 << ADSC);
    while ( ADCSRA & (1 << ADSC) );
    result = ADCH;
    return result;
}
