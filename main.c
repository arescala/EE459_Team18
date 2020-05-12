#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>
#include "encoders.h"
#include "ball_detection.h"
#include "safety_sensor.h"
#include "rc_message.h"

// Output pins
#define BUZZER_PIN PD3

// State machine
#define ON_INIT_STATE 0
#define ON_IDLE_STATE 1
#define ON_BALL_DETECTED_STATE 2
#define ON_LAUNCH_STATE 3
#define ON_CONTROL_FROM_REMOTE_STATE 4
#define ON_MANUAL_LAUNCH_STATE 5
#define LOW_POWER_ERROR_STATE 6

volatile bool ball_detected = false;
volatile int safety_sensor_flag = false;

int main(void){

	// Set output pins
	DDRD |= (1 << BUZZER_PIN);

	init_encoders_motors();
	init_launcher_pwm();
	init_angle_pwm();
	turn_off_launcher();

	unsigned int state = ON_INIT_STATE;

	while (1){

		//not sure where this goes but
		//if rx message, switch states
		//check for message every loop

		if(state == ON_INIT_STATE){
			state = ON_IDLE_STATE;
		}
		else if(state == ON_IDLE_STATE){
			if(/* battery below 14V */){
				state = LOW_POWER_ERROR_STATE;
			}
			else if(/* remote controller on */){
				state = ON_CONTROL_FROM_REMOTE_STATE;
			}
			else if(ball_detected){
				state = ON_BALL_DETECTED_STATE;
				ball_detected = false;
			}
		}
		else if(state == ON_BALL_DETECTED_STATE){
			int front_not_clear = check_surroundings();
			if(!front_not_clear){	// front_not_clear is set when there is an obstruction
				state = ON_LAUNCH_STATE;
			}
			else{
				state = ON_IDLE_STATE;
			}
		}
		else if(state == ON_LAUNCH_STATE){
			turn_on_launcher();
			_delay_ms(5000);
			turn_off_launcher();
			state = ON_IDLE_STATE;
		}
		else if(state == ON_CONTROL_FROM_REMOTE_STATE){
			if(/* remote controller off */){
				state = ON_IDLE_STATE;
			}
			else if(/* launch button pressed && */ ball_detected){
				state = ON_MANUAL_LAUNCH_STATE;
			}
		}
		else if(state == ON_MANUAL_LAUNCH_STATE){
			turn_on_launcher();
			_delay_ms(5000);
			turn_off_launcher();
			state = ON_CONTROL_FROM_REMOTE_STATE;
		}
		else if(state == LOW_POWER_ERROR_STATE){
			if(/* battery above 14V */){
				state = ON_IDLE_STATE;
			}
		}
	}
	


	return state;	// never reached
}
