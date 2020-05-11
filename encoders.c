// File for rotary encoders

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>

/*

Rotary encoders are on the following ports:
  PC5 & PC4 for speed dial
  PC3 & PC2 for pan dial
  PC1 & PB5 for tilt dial

Servos for panning and tilting are on the following ports:
  PB2 (OC1A) for tilting
  PB2 (OC1B) for panning

Servo angles
Tilt
  Only want to go from 15 - 45 degrees

  15 deg : 1.57ms duty cycle
  1.56 ms / 8 us = 196

  30 deg : 1.73 ms duty cycle
  1.73 ms / 8 us = 216

  45 deg : 1.9ms duty cycle
  1.9 ms / 8 us = 237

Pan
  -90 to +90 degrees

  -90 deg : 0.52ms duty cycle
  0.52 ms / 8 us = 65

  -45 deg : 0.96ms duty cycle
  0.96 ms / 8 us = 120

  0 deg : 1.4ms duty cycle
  1.4 ms / 8 us = 175

  90 deg : 2.4ms duty cycle
  2.4 ms / 8 us = 300

*/

//volatile bool speedChanged = false;
//volatile bool panChanged = false;
//volatile bool tiltChanged = false;

volatile unsigned char portBbits, portCbits, speed1, speed2, pan1, pan2, tilt1, tilt2;
volatile unsigned char prevSpeed1, prevSpeed2, prevPan1, prevPan2, prevTilt1, prevTilt2;
volatile int speedCount = 0;
volatile int panCount = 0;
volatile int tiltCount = 0;

int init_pwm(void) {
	// Fast PWM 
	// FOSC = 7372800 Hz
	// Prescalar = 64
	// Fpwm = FOSC / Prescalar * (1 + ICR1)
	// ICR1 is top value to count to
	// We want Fpwm = 50 Hz, so ICR1 = 2303
	// Frequency of clock is FOSC/prescalar = 115200Hz
	// Period is 1/115200 = (approx) 8 us
	ICR1 = 2303;
	OCR1A = 196;  // 15 degrees tilt
	OCR1B = 175;  // pointing forward 0 degrees (pan)
	TCCR1A |= (1 << WGM11 | 1 << COM1A1 | 1 << COM1B1);
	TCCR1B |= (1 << WGM13 | 1 << WGM12 | 1 << CS11 | 1 << CS10); 
}

int init_encoder(void) {

	// Enable pull up resistors on all pins
	PORTC |= (1 << PC5 | 1 << PC4);
	PORTC |= (1 << PC3 | 1 << PC2);
	PORTC |= (1 << PC1);
	PORTB |= (1 << PB5);

	// Initialize values
	portBbits = PINB;
	portCbits = PINC;
	// Init speed
	speed1 = portCbits & (1 << PC5);
	speed2 = portCbits & (1 << PC4);
	prevSpeed2 = speed2;
	prevSpeed1 = speed1;
	// Init pan angle
	pan1 = portCbits & (1 << PC3);
	pan2 = portCbits & (1 << PC2);
	prevPan2 = pan2;
	prevPan1 = pan1;
	// Init tilt angle
	tilt1 = portBbits & (1 << PB5);
	tilt2 = portCbits & (1 << PC1);
	prevTilt2 = tilt2;
	prevTilt1 = tilt1;

	// Initialize pin change interrupts for rotary encoders
	PCICR |= (1 << PCIE0 | 1 << PCIE1); // PCIE0 is for PORTB and PCIE1 is for PORTC
	PCMSK0 |= (1 << PB5);
	PCMSK1 |= (1 << PC5 | 1 << PC4 | 1 << PC3 | 1 << PC2 | 1 << PC1);

	sei();

	while(1){
		/*if(tiltChanged){
			tiltChanged = false;
		}
		if(panChanged){
			panChanged = false;
		}
		if(speedChanged){
			speedChanged = false;
		}*/
	}
}

// ISR for PORTB (half of the tilt dial)
ISR(PCINT0_vect){
	portBbits = PINB;
	tilt1 = portBbits & (1 << PB5);
	if (prevTilt1 != tilt1){
		//tiltChanged = true;
		tiltCount++;
		if(tiltCount > 237){
			tiltCount = 237;
		}
		prevTilt1 = tilt1;
		OCR1A = tiltCount;
	}
}

// ISR for PORTC (speed and pan dials, half of tilt dial)
ISR(PCINT1_vect){
	portCbits = PINC;
	tilt2 = portCbits & (1 << PC1);
	speed1 = portCbits & (1 << PC5);
	speed2 = portCbits & (1 << PC4);
	pan1 = portCbits & (1 << PC3);
	pan2 = portCbits & (1 << PC2);

	if(prevTilt2 != tilt2){
		//tiltChanged = true;
		tiltCount--;
		if(tiltCount < 196){	// cap at 196
			tiltCount = 196;
		}
		prevTilt2 = tilt2;
		OCR1A = tiltCount;
	}
	if(prevSpeed1 != speed1){
		//speedChanged = true;
		speedCount++;
		prevSpeed1 = speed1;
	}
	if(prevSpeed2 != speed2){
		//speedChanged = true;
		speedCount--;
		prevSpeed2 = speed2;
	}
	if(prevPan1 != pan1){
		//panChanged = true;
		panCount++;
		if(panCount > 300){
			panCount = 300;
		}
		prevPan1 = pan1;
		OCR1B = panCount;
	}
	if(prevPan2 != pan2){
		//panChanged = true;
		panCount--;
		if(panCount < 65){
			panCount = 65;
		}
		prevPan2 = pan2;
		OCR1B = panCount;
	}
}
