#ifndef RECEIVER_H
#define RECEIVER_H

#include "VirtualWire/VirtualWire_Config.h"
#include "VirtualWire/VirtualWire.h"
#include "remote/rc_message.h"

#define VW_SPEED 2000
#define RC_TIMEOUT_THRESHOLD 0xf8

//input pins
#define RF_PIN      PD6

uint8_t rx_buf[RC_MESSAGE_LEN];
uint8_t rx_buflen;

extern volatile uint8_t time_since_rx;

void rx_setup();

uint8_t rc_recv(struct RCMessage* message);

#endif
