//  File for ball detection

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ball_detection.h"


/*
 
 Ball detection limit switch is on the following port:
    PD0 PCINT18 (Pin Change Interrupt 18) -> PCMSK2
 
 Servo for ball release is located on the following port:
    PD1

 Servo angle
    -90 to 90 degrees
 125 - 218 cycles
 
 */

volatile unsigned char portBbits, servo_rotate;

void init_detection_pwm(void) {
    // Fast PWM
    // FOSC = 7372800 Hz
    // Prescalar = 64
    // Fpwm = FOSC / Prescalar * 256
    OCR0A = 250; // Have dispenser oriented on one end of release mechanism
    TCCR0A |= (1 << COM0A1 | 1 << COM0A0 | 1 << WGM01 | 1 << WGM00);
    TCCR0B |= (1 << WGM02 | 1 << CS01 | 1 << CS02);

}

void init_detection(void) {
    // Set output pins
    DDRD |= (1 << PD1);
    
    // Initialize sleep mode for power conservation
    SMCR |= (1 << SE);
    // Initialize pin change interrupts for limit switch
    PCMSK2 |= (1 << PD0);
    PCICR |= (1 << PCIE2); // PCIE2 is for PORTD
    
    sei();
}

// ISR for PORTD (servo releasing mechanism) in encoders.c

