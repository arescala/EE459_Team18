#include "PCA9685_driver.h"

void PCA9685_init()
{
    i2c_init(BDIV);
    PCA9685_reset();
    //set default frequency
    setPWMFreq(1000);
}

void PCA9685_reset()
{
    PCA9685_write8(PCA9685_MODE1, MODE1_RESTART);
    delay(2);
}

void setPWMFreq(float freq)
{
    if (freq < 1)
        freq = 1;
    if (freq > 3500)
        freq = 3500;

    float prescaleval = ((PCA9685_oscillator_frequency / (freq * 4096.0)) + 0.5) - 1;
    if (prescaleval < PCA9685_PRESCALE_MIN)
        prescaleval = PCA9685_PRESCALE_MIN;
    if (prescaleval > PCA9685_PRESCALE_MAX)
        prescaleval = PCA9685_PRESCALE_MAX;
    uint8_t prescale = (uint8_t)prescaleval;

    uint8_t oldmode = PCA9685_read8(PCA9685_MODE1);
    uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
    PCA9685_write8(PCA9685_MODE1, newmode); // go to sleep
    PCA9685_write8(PCA9685_PRESCALE, prescale); // set the prescaler
    PCA9685_write8(PCA9685_MODE1, oldmode);
    delay(5);
    // This sets the MODE1 register to turn on auto increment.
    PCA9685_write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  on At what point in the 4096-part cycle to turn the PWM output ON
 *  @param  off At what point in the 4096-part cycle to turn the PWM output OFF
 */
void setPWM(uint8_t num, uint16_t on, uint16_t off)
{
    uint8_t buf[4];
    uint8_t addr = PCA9685_LED0_ON_L + 4 * num;
    buf[0] = on;
    buf[1] = on >> 8;
    buf[2] = off;
    buf[3] = off >> 8;
    i2c_io(PCA9685_I2C_ADDRESS, &addr, 1, NULL, 0, buf, 4);
}

/*!
 *   @brief  Helper to set pin PWM output. Sets pin without having to deal with
 * on/off tick placement and properly handles a zero value as completely off and
 * 4095 as completely on.  Optional invert parameter supports inverting the
 * pulse for sinking to ground.
 *   @param  num One of the PWM output pins, from 0 to 15
 *   @param  val The number of ticks out of 4096 to be active, should be a value
 * from 0 to 4095 inclusive.
 *   @param  invert If true, inverts the output, defaults to 'false'
 */
void setPin(uint8_t num, uint16_t val, uint8_t invert)
{
    // Clamp value between 0 and 4095 inclusive.
    if (val > (uint16_t) 4095)
    {
        val = (uint16_t) 4095;
    }

    if (invert)
    {
        if (val == 0)
        {
            // Special value for signal fully on.
            setPWM(num, 4096, 0);
        }
        else if (val == 4095)
        {
            // Special value for signal fully off.
            setPWM(num, 0, 4096);
        }
        else
        {
            setPWM(num, 0, 4095 - val);
        }
    }
    else
    {
        if (val == 4095)
        {
            // Special value for signal fully on.
            setPWM(num, 4096, 0);
        }
        else if (val == 0)
        {
            // Special value for signal fully off.
            setPWM(num, 0, 4096);
        }
        else
        {
            setPWM(num, 0, val);
        }
    }
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins based on the input
 * microseconds, output is not precise
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @param  microseconds The number of microseconds to turn the PWM output ON
 */
void writeMicroseconds(uint8_t num, uint16_t microseconds)
{
    double pulse = microseconds;
    double pulselength;
    pulselength = 1000000; // 1,000,000 us per second

    // Read prescale
    uint16_t prescale = readPrescale();

    // Calculate the pulse for PWM based on Equation 1 from the datasheet section
    // 7.3.5
    prescale += 1;

    pulselength *= prescale;
    pulselength /= PCA9685_oscillator_frequency;
    pulse /= pulselength;

    setPWM(num, 0, pulse);
}

/*!
 *  @brief  Sets the output mode of the PCA9685 to either
 *  open drain or push pull / totempole.
 *  Warning: LEDs with integrated zener diodes should
 *  only be driven in open drain mode.
 *  @param  totempole Totempole if true, open drain if false.
 */
void setOutputMode(uint8_t totempole) {
    uint8_t oldmode = PCA9685_read8(PCA9685_MODE2);
    uint8_t newmode;
    if (totempole) {
        newmode = oldmode | MODE2_OUTDRV;
    } else {
        newmode = oldmode & ~MODE2_OUTDRV;
    }
    PCA9685_write8(PCA9685_MODE2, newmode);
}

uint8_t readPrescale() {
    return PCA9685_read8(PCA9685_PRESCALE);
}

//low level functions for i2c
uint8_t PCA9685_read8(uint8_t addr)
{
    uint8_t buf = 0;
    /*uint8_t status =*/ i2c_io(PCA9685_I2C_ADDRESS, &addr, 1, &buf, 1, NULL, 0);
    return buf;
}

void PCA9685_write8(uint8_t addr, uint8_t data)
{
    /*uint8_t status =*/ i2c_io(PCA9685_I2C_ADDRESS, &addr, 1, NULL, 0, &data, 1);
}

void delay(uint32_t x)
{
    int i;
    for (i = 0; i < x*100; i += 1)
        asm("nop"::); //small delay
}
