#include "led_driver.h"

void set_speed_indicator(uint8_t speed)
{
    if (speed == 255)
    {
        led_on(0); led_on(1); led_on(2);
        led_on(3); led_on(4); led_on(5);
    }
    else if (speed > 204)
    {
        led_on(0); led_on(1); led_on(2);
        led_on(3); led_on(4); led_off(5);
    }
    else if (speed > 153)
    {
        led_on(0); led_on(1); led_on(2);
        led_on(3); led_off(4); led_off(5);
    }
    else if (speed > 102)
    {
        led_on(0); led_on(1); led_on(2);
        led_off(3); led_off(4); led_off(5);
    }
    else if (speed > 51)
    {
        led_on(0); led_on(1); led_off(2);
        led_off(3); led_off(4); led_off(5);
    }
    else
    {
        led_on(0); led_off(1); led_off(2);
        led_off(3); led_off(4); led_off(5);
    }
}

void set_tilt_indicator(uint8_t tilt)
{
    if (tilt > 204)
    {
        led_off(6); led_off(7); led_off(8);
        led_off(9); led_on(10);
    }
    else if (tilt > 153)
    {
        led_off(6); led_off(7); led_off(8);
        led_on(9); led_off(10);
    }
    else if (tilt > 102)
    {
        led_off(6); led_off(7); led_on(8);
        led_off(9); led_off(10);
    }
    else if (tilt > 51)
    {
        led_off(6); led_on(7); led_off(8);
        led_off(9); led_off(10);
    }
    else
    {
        led_on(6); led_off(7); led_off(8);
        led_off(9); led_off(10);
    }
}
void set_battery_indicator(uint8_t battery)
{
    if (battery > 204)
    {
        led_on(11); led_on(12); led_on(13);
        led_on(14); led_on(15);
    }
    else if (battery > 153)
    {
        led_on(11); led_on(12); led_on(13);
        led_on(14); led_off(15);
    }
    else if (battery > 102)
    {
        led_on(11); led_on(12); led_on(13);
        led_off(14); led_off(15);
    }
    else if (battery > 51)
    {
        led_on(11); led_on(12); led_off(13);
        led_off(14); led_off(15);
    }
    else
    {
        led_on(11); led_off(12); led_off(13);
        led_off(14); led_off(15);
    }
}

void led_on(uint8_t num)
{
    setPin(num, LED_ON_VALUE, 0);
}
void led_off(uint8_t num)
{
    setPin(num, LED_OFF_VALUE, 0);
}

void init_led_driver()
{
    PCA9685_init();
}
