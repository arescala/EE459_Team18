#include <util/delay.h>
#include <avr/io.h>

#include "rc_message.h"
#include "transmitter.h"
#include "dials.h"

#define SEND_FREQ 10 //Hz

int main()
{
    dials_setup();
    //init RF TX
    tx_setup();

    struct RCMessage message;

    //loop, sending current state
    while(1)
    {
        //gather state
        message.speed = get_speed();
        message.pan = get_pan();
        message.tilt = get_tilt();
        message.launch = get_launch();
        //send as datagrams
        send(&message);

        //sleep for period of time
        _delay_ms(1000/SEND_FREQ);
    }
}
