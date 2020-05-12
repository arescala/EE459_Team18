#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>
#include "encoders.h"
#include "ball_detection.h"
#include "safety_sensor.h"
#include "remote/rc_message.h"
#include "receiver.h"
#include "led_driver.h"
#include "battery_monitor.h"
#include "PCA9685/PCA9685_driver.h"

// Output pins
#define BUZZER_PIN PD3
#define ALERT_LED_PIN PD4

// State machine
#define ON_INIT_STATE 0
#define ON_IDLE_STATE 1
#define ON_BALL_DETECTED_STATE 2
#define ON_LAUNCH_STATE 3
#define ON_CONTROL_FROM_REMOTE_STATE 4
#define ON_MANUAL_LAUNCH_STATE 5
#define LOW_POWER_ERROR_STATE 6

volatile bool ball_detected = false;
volatile int safety_sensor_flag = 0;
volatile uint8_t time_since_rx = 0;

uint8_t battery_voltage = 0;

uint8_t rc_connected = 0;
uint8_t setting_tilt = 0;
uint8_t setting_pan = 0;
uint8_t setting_speed = 0;
uint8_t setting_launch = 0;

int main(void){

	// Set output pins
	DDRD |= (1 << BUZZER_PIN);
	DDRD |= (1 << ALERT_LED_PIN);

	init_encoders_motors();
	init_launcher_pwm();
	init_angle_pwm();
	turn_off_launcher();

	rx_setup();
	init_battery_monitor();

	unsigned int state = ON_INIT_STATE;

	struct RCMessage message;

	while (1){

		//check for message every loop
		if (rc_recv(&message))
		{
			setting_tilt = message.tilt;
			setting_pan = message.pan;
			setting_speed = message.speed;
			setting_launch = message.launch;
			//reset timeout counter
			rc_connected = 1;
			time_since_rx = 0;
		}
		else
		{
			//no message received, check timeout
			//if timed out, set rc_connected to 0
			if (time_since_rx >= RC_TIMEOUT_THRESHOLD)
			{
				rc_connected = 0;
			}

		}

		//check battery voltage
		battery_voltage	= get_battery_voltage();

		//state machine

		if(state == ON_INIT_STATE){
			state = ON_IDLE_STATE;
		}
		else if(state == ON_IDLE_STATE){
			if(battery_voltage < BATTERY_LOW_THRESHOLD){
				state = LOW_POWER_ERROR_STATE;
			}
			else if(rc_connected){
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
			PORTD |= (1 << BUZZER_PIN);
			_delay_ms(3401);	// delay to create buzzer frequency
			PORTD &= ~(1 << BUZZER_PIN);

			turn_on_launcher();
			_delay_ms(5000);
			turn_off_launcher();

			state = ON_IDLE_STATE;
		}
		else if(state == ON_CONTROL_FROM_REMOTE_STATE){
			if(battery_voltage < BATTERY_LOW_THRESHOLD){
				state = LOW_POWER_ERROR_STATE;
			}
			else if(!rc_connected){
				state = ON_IDLE_STATE;
			}
			else if(setting_launch && ball_detected){
				state = ON_MANUAL_LAUNCH_STATE;
			}
		}
		else if(state == ON_MANUAL_LAUNCH_STATE){
			PORTD |= (1 << BUZZER_PIN);
			_delay_ms(3401);	// delay to create buzzer frequency
			PORTD &= ~(1 << BUZZER_PIN);

			turn_on_launcher();
			_delay_ms(5000);
			turn_off_launcher();
			state = ON_CONTROL_FROM_REMOTE_STATE;
		}
		else if(state == LOW_POWER_ERROR_STATE){
			PORTD |= (1 << BUZZER_PIN);
			_delay_ms(2146);	// delay to create buzzer frequency, different from launch sound
			PORTD &= ~(1 << BUZZER_PIN);	// turn LED on

			PORTD |= (1 << ALERT_LED_PIN);
			if(battery_voltage > BATTERY_GOOD_THRESHOLD){
				PORTD &= ~(1 << ALERT_LED_PIN);	// turn LED off
				state = ON_IDLE_STATE;
			}
		}
	}
	


	return state;	// never reached
}
