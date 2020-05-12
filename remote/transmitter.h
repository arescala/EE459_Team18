#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include "../VirtualWire/VirtualWire_Config.h"
#include "../VirtualWire/VirtualWire.h"
#include "rc_message.h"

#define VW_SPEED 2000

//output pins
#define RF_PIN      PD0

uint8_t tx_buf[RC_MESSAGE_LEN];
uint8_t tx_buflen;

void tx_setup();

void rc_send(struct RCMessage* message);

#endif
