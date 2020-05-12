#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdlib.h>
#include <avr/io.h>

#include "PCA9685/PCA9685_driver.h"

#define LED_ORDER_SPEED     0
#define LED_ORDER_PAN       1
#define LED_ORDER_TILT      2
#define LED_ORDER_BATTERY   3

#define LED_ON_VALUE 2048
#define LED_OFF_VALUE 0

//all of the indicators expect uint8_t
//with 0 being 0% and 256 being 100%

void set_speed_indicator(uint8_t speed);
void set_tilt_indicator(uint8_t tilt);
void set_battery_indicator(uint8_t battery);

void led_on(uint8_t num);
void led_off(uint8_t num);

void init_led_driver();

#endif
