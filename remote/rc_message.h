#ifndef RC_MESSAGE_H
#define RC_MESSAGE_H

#include <stdint.h>

#define RC_MESSAGE_LEN 4

struct RCMessage
{
    uint8_t speed;
    uint8_t pan;
    uint8_t tilt;
    uint8_t launch;
};

void rc_message_to_byte_array(struct RCMessage* rc_message, uint8_t* buf);

void byte_array_to_rc_message(uint8_t* buf, struct RCMessage* rc_message);


#endif
