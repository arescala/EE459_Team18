#include "receiver.h"

void rx_setup()
{
    vw_setup(VW_SPEED);
    vw_rx_start();
    rx_buflen = sizeof(rx_buf);

    TCCR2A |= (1 << WGM21); //CTC
    TIMSK2 |= (1 << OCIE2A); //enable interrupts
    OCR2A = 0xff; //compare against 255
    TCCR2B |= (1 << CS22 | 1 << CS21 | 1 << CS20); //1024 prescaler
}

uint8_t rc_recv(struct RCMessage* message)
{
    //check for message and if message received is valid
    if (vw_have_message() && vw_get_message(rx_buf, &rx_buflen))
    {
        //store in message and report goodness
        byte_array_to_rc_message(rx_buf, message);
        return 1; //aka valid message
    }
    else
    {
        return 0; //invalid message, don't read
    }
}

ISR(TIMER2_COMPA_vect)
{
    time_since_rx += 1;
}
