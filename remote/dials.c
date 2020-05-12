#include "dials.h"

void dials_setup()
{
    //pullups
    PORTC |= (1 << SPEED_PIN) | (1 << PAN_PIN) | (1 << TILT_PIN);
    PORTD |= (1 << LAUNCH_PIN);

    ADMUX |= (REFS0);  //set ADC reference to AVCC
    ADMUX |= (1 << ADLAR); // 8-bit conversions, 8 is enough

    // 7372800/128 = 57.6 kHz, good for ADC
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescalar = 128
    ADCSRA |= (1 << ADEN); // enable ADC
}

uint8_t get_speed()
{
    return adc_sample(SPEED_ADC_CHANNEL);
}

uint8_t get_tilt()
{
    return adc_sample(TILT_ADC_CHANNEL);
}

uint8_t get_pan()
{
    return adc_sample(PAN_ADC_CHANNEL);
}

uint8_t get_launch()
{
    uint8_t pressed = PIND & (1 << LAUNCH_PIN);

    if (!DEBOUNCE_LAUNCH)
    {
        return pressed;
    }
    else
    {
        if (pressed == 0)
        {
            _delay_ms(5);
            if ((PIND & (1 << LAUNCH_PIN)) == 0)
                return 1;
            else
                return 0;
        }
    }
}

uint8_t adc_sample(uint8_t channel)
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
