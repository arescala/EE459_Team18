// File for rotary encoders

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "encoders.h"
#include "led_driver.h"

#define LAUNCHER_PIN PD6
#define SWITCH_PRESSED !(PIND & (1 << PD0))

/*

Rotary encoders are on the following ports:
  PB7 & PD5 for speed dial
  PC3 & PC2 for pan dial
  PC1 & PB0 for tilt dial

Servos for panning and tilting are on the following ports:
  PB1 (OC1A) for tilting
  PB2 (OC1B) for panning

Servo angles
Tilt
  Only want to go from 15 - 45 degrees
  198 - 218 cycles (delta 20)
  delta 1 cycles = delta 12 for indicator

Pan
  -90 to +90 degrees
  125 - 250 cycles

Speed
  0% to 100% rpm
  28 - 57 cycles
  delta 1 cycles = delta 8 for 

*/
extern volatile bool ball_detected;
volatile unsigned char portDbits, portBbits, portCbits, speed1, speed2, pan1, pan2, tilt1, tilt2;
volatile unsigned char prevSpeed1, prevSpeed2, prevPan1, prevPan2, prevTilt1, prevTilt2;
volatile int speedCount = 0;
volatile int panCount = 0;
volatile int tiltCount = 0;
volatile uint8_t speed_indicator, pan_indicator, tilt_indicator;

void init_launcher_pwm(void) {
	// Fast PWM 
	// FOSC = 7372800 Hz
	// Prescalar = 256
	// Fpwm = FOSC / Prescalar * 256
	// Fpwm = 112.5 Hz
	// Frequency of clock is FOSC/prescalar = 28800Hz
	// Period is 1/28800 = (approx) 0.00003472s

	// For range of speeds the OCR0A value should go from
	// 1ms/0.00003472s = 28 
	// to
	// 2ms/0/00003472s = 57
	TCCR0A |= (1 << COM0A1 | 1 << WGM01 | 1 << WGM00);
	OCR0A = 43; // Set to 50% duty cycle or 1.5ms
	speed_indicator = 112; // (57-43) * 8
	set_speed_indicator(speed_indicator);
}

void turn_on_launcher(void){
	TCCR0B |= (1 << CS02);
}

void turn_off_launcher(void){
	TCCR0B &= ~(1 << CS02);
}

void init_angle_pwm(void) {
	// Fast PWM 
	// FOSC = 7372800 Hz
	// Prescalar = 64
	// Fpwm = FOSC / Prescalar * (1 + ICR1)
	// ICR1 is top value to count to
	// We want Fpwm = 50 Hz, so ICR1 = 2303
	// Frequency of clock is FOSC/prescalar = 115200Hz
	// Period is 1/115200 = (approx) 8 us
	ICR1 = 2303;
	OCR1A = 198;  // 15 degrees tilt
	OCR1B = 187;  // 0 degrees pan

	tilt_indicator = 0; // lowest tilt
	set_tilt_indicator(tilt_indicator);

	TCCR1A |= (1 << WGM11 | 1 << COM1A1 | 1 << COM1B1);
	TCCR1B |= (1 << WGM13 | 1 << WGM12 | 1 << CS11 | 1 << CS10); 
}

void init_encoders_motors(void) {

	// Enable pull up resistors on all pins
	PORTC |= (1 << PC3 | 1 << PC2);
	PORTC |= (1 << PC1);
	PORTB |= (1 << PB0 | 1 << PB7);
	PORTD |= (1 << PD5);

	// Set output pins
	DDRB |= (1 << PB1 | 1 << PB2);
	DDRD |= (1 << LAUNCHER_PIN);

	// Initialize values
	portBbits = PINB;
	portCbits = PINC;
	portDbits = PIND;
	// Init speed
	speed1 = portBbits & (1 << PB7);
	speed2 = portDbits & (1 << PD5);
	prevSpeed2 = speed2;
	prevSpeed1 = speed1;
	// Init pan angle
	pan1 = portCbits & (1 << PC3);
	pan2 = portCbits & (1 << PC2);
	prevPan2 = pan2;
	prevPan1 = pan1;
	// Init tilt angle
	tilt1 = portBbits & (1 << PB0);
	tilt2 = portCbits & (1 << PC1);
	prevTilt2 = tilt2;
	prevTilt1 = tilt1;

	// Initialize pin change interrupts for rotary encoders
	PCICR |= (1 << PCIE0 | 1 << PCIE1 | 1 << PCIE2); // PCIE0 is for PORTB and PCIE1 is for PORTC
	PCMSK0 |= (1 << PB0 | 1 << PB7);
	PCMSK1 |= (1 << PC3 | 1 << PC2 | 1 << PC1);
	PCMSK2 |= (1 << PD5);

	sei();
}

// ISR for PORTB (half of the tilt dial)
ISR(PCINT0_vect){
	portBbits = PINB;
	tilt1 = portBbits & (1 << PB0);
	speed1 = portBbits & (1 << PB7);
	if (prevTilt1 != tilt1){
		tiltCount++;
		tilt_indicator += 12;
		if(tiltCount > 218){
			tiltCount = 218;
			tilt_indicator -= 12;
		}
		set_tilt_indicator(tilt_indicator);
		prevTilt1 = tilt1;
		OCR1A = tiltCount;
	}

	if(prevSpeed1 != speed1){
		speedCount++;
		speed_indicator += 8;
		if(speedCount > 14){
			speedCount = 14;
			speed_indicator -= 8;
		}
		set_speed_indicator(speed_indicator);
		prevSpeed1 = speed1;
		OCR0A = speedCount;
	}
}

// ISR for PORTC (speed and pan dials, half of tilt dial)
ISR(PCINT1_vect){
	portCbits = PINC;
	tilt2 = portCbits & (1 << PC1);
	pan1 = portCbits & (1 << PC3);
	pan2 = portCbits & (1 << PC2);

	if(prevTilt2 != tilt2){
		tiltCount--;
		tilt_indicator -= 12;
		if(tiltCount < 198){	// cap at 196
			tiltCount = 198;
			tilt_indicator += 12;
		}
		set_tilt_indicator(tilt_indicator);
		prevTilt2 = tilt2;
		OCR1A = tiltCount;
	}
	if(prevPan1 != pan1){
		panCount++;
		if(panCount > 250){
			panCount = 250;
		}
		prevPan1 = pan1;
		OCR1B = panCount;
	}
	if(prevPan2 != pan2){
		panCount--;
		if(panCount < 125){
			panCount = 125;
		}
		prevPan2 = pan2;
		OCR1B = panCount;
	}
}

ISR(PCINT2_vect){
	portDbits = PIND;
	speed2 = portDbits & (1 << PD5);
	if(prevSpeed2 != speed2){
		speedCount--;
		speed_indicator -= 8;
		if(speedCount < 7){
			speedCount = 7;
			speed_indicator += 8;
		}
		set_speed_indicator(speed_indicator);
		prevSpeed2 = speed2;
		OCR0A = speedCount;
	}

	if (SWITCH_PRESSED){
        OCR1A = 125;    // Rotate servo 180 degrees so that ball can be dispensed into the launcher
        ball_detected = true;   // flag to alert main.c that ball is in launcher
        _delay_ms(5000);    // Wait 5 seconds before returning to original positon
        OCR1A = 250;    // Servo goes to original position
    }
}