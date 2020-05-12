#ifndef RC_MESSAGE_C
#define RC_MESSAGE_C

#include "rc_message.h"



void rc_message_to_byte_array(struct RCMessage* rc_message, uint8_t* buf)
{
    buf[0] = rc_message->speed;
    buf[1] = rc_message->pan;
    buf[2] = rc_message->tilt;
    buf[3] = rc_message->launch;
}

void byte_array_to_rc_message(uint8_t* buf, struct RCMessage* rc_message)
{
    rc_message->speed = buf[0];
    rc_message->pan = buf[1];
    rc_message->tilt = buf[2];
    rc_message->launch = buf[2];
}

#endif
