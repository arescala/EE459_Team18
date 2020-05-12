#include "transmitter.h"

void tx_setup()
{
    vw_setup(VW_SPEED);
    DDRD |= (1 << PD0);
    tx_buflen = sizeof(tx_buf);
}

void send(struct RCMessage* message)
{
    rc_message_to_byte_array(message, tx_buf);
    vw_send(tx_buf, tx_buflen);
}
