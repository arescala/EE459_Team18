//  File for ball detection

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "ball_detection.hpp"

#define SWITCH_PRESSED !(PIND & (1 << PD0))
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
    
    ICR1 = 2303;
    OCR1A = 250; // Have dispenser oriented on one end of release mechanism
    TCCR1A |= (1 << WGM11 | 1 << COM1A1 | 1 << COM1B1);
    TCCR1B |= (1 << WGM13 | 1 << WGM12 | 1 << CS11 | 1 << CS10);
    
}

void init_detection(void) {
    // Set output pins
    DDRD |= (1 << PD1);
    
    // Initialize sleep mode for power conservation
    SMCR |= (1 << SE);
    // Initialize pin change interrupts for limit switch
    PCMSK2 |= (1 << PD0);
    PCICR |= (1 << PCIE2); // PCIE2 is for PORTD
    
    sleep_enable();
    sei();
}

// ISR for PORTD (servo releasing mechanism)
ISR(PCINT2_vect){
    if (SWITCH_PRESSED){
        OCR1A = 125;    // Rotate servo 180 degrees so that ball can be dispensed into the launcher
        _delay_ms(5000);    // Wait 5 seconds before returning to original positon
        OCR1A = 250;    // Servo goes to original position
        }
}
