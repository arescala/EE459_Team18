#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <avr/io.h>

#define BATTERY_GOOD_THRESHOLD 200
#define BATTERY_LOW_THRESHOLD 150

uint8_t get_battery_voltage();


#endif
