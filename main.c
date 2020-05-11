#include <avr/io.h>
#include <stdio.h>
#include "encoders.h"

// Output pins
#define LAUNCHER_MOTOR_PIN PD2
#define BUZZER_PIN PD3

// Input pins
#define LOAD_PIN PB6
#define SAFETY_PIN PB4

// State machine
#define ON_INIT_STATE 0
#define ON_IDLE_STATE 1
#define ON_BALL_DETECTED_STATE 2
#define ON_LAUNCH_STATE 3
#define ON_CONTROL_FROM_REMOTE_STATE 4
#define ON_MANUAL_LAUNCH_STATE 5
#define LOW_POWER_ERROR_STATE 6

int main(void){

	// Set output pins
	DDRD |= (1 << LAUNCHER_MOTOR_PIN);
	DDRD |= (1 << BUZZER_PIN);

	init_pwm();
	init_encoder();

	unsigned int state = ON_INIT_STATE;

	
	
	return state;
}