#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdlib.h>
#include <avr/io.h>

#include "PCA9685/PCA9685_driver.h"

//all of the indicators expect uint8_t
//with 0 being 0% and 256 being 100%

void set_speed_indicator(uint8_t speed);
void set_pan_indicator(uint8_t pan);
void set_tilt_indicator(uint8_t tilt);
void set_battery_indicator(uint8_t battery);

#endif
