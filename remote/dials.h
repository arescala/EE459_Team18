#include <avr/io.h>
#include <util/delay.h>

//input pins
#define SPEED_PIN   PC0
#define PAN_PIN     PC1
#define TILT_PIN    PC2
#define LAUNCH_PIN  PD1
#define SPEED_ADC_CHANNEL 0x00
#define PAN_ADC_CHANNEL 0x01
#define TILT_ADC_CHANNEL 0x02

#define DEBOUNCE_LAUNCH 0

void dials_setup();

uint8_t get_speed();

uint8_t get_tilt();

uint8_t get_pan();

uint8_t get_launch();

uint8_t adc_sample(uint8_t channel);
