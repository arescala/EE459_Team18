#include <avr/io.h>
#include <stdio.h>

// Output pins
#define PANNING_SERVO_PIN PB2
#define TILTING_SERVO_PIN PB1
#define LAUNCHER_MOTOR_PIN PD2
#define BUZZER_PIN PD3

// Input pins
#define LOAD_PIN PB6
#define SAFETY_PIN PB4

// State machine
#define ON_IDLE_STATE 1
#define ON_LAUNCH_STATE 2
#define ERROR_STATE 3

int main(void){
	// Initialize outputs to 0
	PORTD = 0x00;

	// Set output pins
	DDRD |= (1 << PANNING_SERVO_PIN);
	DDRD |= (1 << TILTING_SERVO_PIN);
	DDRD |= (1 << LAUNCHER_MOTOR_PIN);
	DDRD |= (1 << BUZZER_PIN);

	unsigned int state = 0;
	
}