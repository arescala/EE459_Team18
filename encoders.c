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

*/

volatile bool speedChanged = false;
volatile bool panChanged = false;
volatile bool tiltChanged = false;

volatile unsigned char portBbits, portCbits, speed1, speed2, pan1, pan2, tilt1, tilt2;
volatile unsigned char prevSpeed1, prevSpeed2, prevPan1, prevPan2, prevTilt1, prevTilt2;
volatile int speedCount = 0;
volatile int panCount = 0;
volatile int tiltCount = 0;

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
		if(tiltChanged){
			tiltChanged = false;
		}
		if(panChanged){
			panChanged = false;
		}
		if(speedChanged){
			speedChanged = false;
		}
	}
}

// ISR for PORTB (half of the tilt dial)
ISR(PCINT0_vect){
	portBbits = PINB;
	tilt1 = portBbits & (1 << PB5);
	if (prevTilt1 != tilt1){
		tiltChanged = true;
		tiltCount++;
		prevTilt1 = tilt1;
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
		tiltChanged = true;
		tiltCount--;
		prevTilt2 = tilt2;
	}
	if(prevSpeed1 != speed1){
		speedChanged = true;
		speedCount++;
		prevSpeed1 = speed1;
	}
	if(prevSpeed2 != speed2){
		speedChanged = true;
		speedCount--;
		prevSpeed2 = speed2;
	}
	if(prevPan1 != pan1){
		panChanged = true;
		panCount++;
		prevPan1 = pan1;
	}
	if(prevPan2 != pan2){
		panChanged = true;
		panCount--;
		prevPan2 = pan2;
	}
}




