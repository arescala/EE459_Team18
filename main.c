#include <avr/io.h>
#include <stdio.h>

// Output pins
#define SERVO1_PIN PD2
#define SERVO2_PIN PD3
#define BUZZER_PIN PD4

// Input pins
#define ANGLE_PIN PC0
#define SPEED_PIN PC1
#define LOAD_PIN PC2
#define SAFETY_PIN PC3

#define OFF_STATE 0
#define ON_IDLE_STATE 1
#define ON_LAUNCH_STATE 2
#define ERROR_STATE 3

int main(void){
	// Initialize outputs to 0
	PORTD = 0x00;

	// Set output pins
	DDRD |= (1 << SERVO1_PIN);
	DDRD |= (1 << SERVO2_PIN);
	DDRD |= (1 << BUZZER_PIN);

	unsigned int state = 0;
	
}