// File for safety sensor

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "safety_sensor.h"

#define PING_SENSOR PD7 // Ultrasonic sensor is on PD7

/*
 COMMUNICATION PROTOCOL FROM SENSOR DATASHEET:
    1. Host device sends triggger pulse 5 us
    2. Wait for echo pulse to return - maximum return time is 18.5 ms (18.5 ms == 3m)

 */

volatile int sensor_flag = 0;

int check_surroundings() {
    DDRD |= (1 << PING_SENSOR); // Set sensor pin to output to send out pulse
    PORTD |= (1 << PING_SENSOR); // Send trigger pulse
    _delay_us(5);   // Delay 5 us as directed in datasheet
    DDRD |= (0 << PING_SENSOR); // Revert pin back to input so that we can receive data
    TCCR0 |= (1 << WGM00); // Start timer
    while ((PIND && PING_SENSOR) == 1) {
        TCCR0 = 0; // Stop the timer
        if (TCNT0 <= 18500) { // If the timer value is less than or equal to the maximum sensor distance
            sensor_flag = 1; // Send flag stating that there is an object within launch radius
            return sensor_flag;
        }
    }
}
