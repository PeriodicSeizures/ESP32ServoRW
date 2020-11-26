#include "ESP32ServoRW.h"
#include "Arduino.h"

//
Servo::Servo(int channel)
{		// initialize this channel with plausible values, except pin # (we set pin # when attached)
	REFRESH_CPS = 50;
	this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
	this->timer_width = DEFAULT_TIMER_WIDTH;
	this->pinNumber = -1;     // make it clear that we haven't attached a pin to this channel
	this->min = DEFAULT_uS_LOW;
	this->max = DEFAULT_uS_HIGH;
	this->timer_width_ticks = pow(2,this->timer_width);
  this->channel = channel;
}

int Servo::attach(int pin)
{

    return (this->attach(pin, DEFAULT_uS_LOW, DEFAULT_uS_HIGH));
}

int Servo::attach(int pin, int min, int max)
{

#ifdef ENFORCE_PINS
        // Recommend only the following pins 2,4,12-19,21-23,25-27,32-33
        if (pin == 2 || pin == 4 || pin == 5 || (pin>=12 && pin<=19) || (pin>=21 && pin<=23) || (pin>=25 && pin<=27) || pin==32 || pin==33)
        {
#endif

            // OK to proceed; first check for new/reuse
            if (this->pinNumber < 0) // we are attaching to a new or previously detached pin; we need to initialize/reinitialize
            {
                this->ticks = DEFAULT_PULSE_WIDTH_TICKS;
                this->timer_width = DEFAULT_TIMER_WIDTH;
                this->timer_width_ticks = pow(2,this->timer_width);
            }
            this->pinNumber = pin;
#ifdef ENFORCE_PINS
        }
        else
        {
            Serial.println("This pin can not be a servo: "+String(pin)+"\r\nServo availible on: 2,4,5,12-19,21-23,25-27,32-33");
            return 0;
        }
#endif


        // min/max checks 
        if (min < MIN_PULSE_WIDTH)          // ensure pulse width is valid
            min = MIN_PULSE_WIDTH;
        if (max > MAX_PULSE_WIDTH)
            max = MAX_PULSE_WIDTH;
        this->min = min;     //store this value in uS
        this->max = max;    //store this value in uS
        // Set up this channel
        // if you want anything other than default timer width, you must call setTimerWidth() before attach
        //pwm.attachPin(this->pinNumber, REFRESH_CPS, this->timer_width );   // GPIO pin assigned to channel
        ledcSetup(this->channel, REFRESH_CPS, this->timer_width);
        ledcAttachPin(this->pinNumber, this->channel);
        this->isAttached = true;
        //Serial.println("Attaching servo : "+String(pin)+" on PWM "+String(pwm.getChannel()));
        return 1;
}

void Servo::detach()
{
    if (this->attached())
    {
        //keep track of detached servos channels so we can reuse them if needed
        //pwm.detachPin(this->pinNumber);
        ledcDetachPin(this->pinNumber);

        this->pinNumber = -1;
        this->isAttached = false;
    }
}

void Servo::write(int value)
{
    // treat values less than MIN_PULSE_WIDTH (500) as angles in degrees (valid values in microseconds are handled as microseconds)
    if (value < MIN_PULSE_WIDTH)
    {
        if (value < 0)
            value = 0;
        else if (value > 180)
            value = 180;

        value = map(value, 0, 180, this->min, this->max);
    }
    this->writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value)
{
    // calculate and store the values for the given channel
    if (this->attached())   // ensure channel is valid
    {
        if (value < this->min)          // ensure pulse width is valid
            value = this->min;
        else if (value > this->max)
            value = this->max;

        value = usToTicks(value);  // convert to ticks
        this->ticks = value;
        // do the actual write
        //pwm.write( this->ticks);
        ledcWrite(this->channel, this->ticks);
    }
}

int Servo::read() // return the value as degrees
{
    return (map(readMicroseconds()+1, this->min, this->max, 0, 180));
}

int Servo::readMicroseconds()
{
    int pulsewidthUsec;
    if (this->attached())
    { 
        pulsewidthUsec = ticksToUs(this->ticks);
    }
    else
    {
        pulsewidthUsec = 0;
    }

    return (pulsewidthUsec);
}

bool Servo::attached()
{
    return this->isAttached;
}

void Servo::setTimerWidth(int value)
{
    // only allow values between 16 and 20
    if (value < 16)
        value = 16;
    else if (value > 20)
        value = 20;
        
    // Fix the current ticks value after timer width change
    // The user can reset the tick value with a write() or writeUs()
    int widthDifference = this->timer_width - value;
    // if positive multiply by diff; if neg, divide
    if (widthDifference > 0)
    {
        this->ticks = widthDifference * this->ticks;
    }
    else if (widthDifference < 0)
    {
        this->ticks = this->ticks/-widthDifference;
    }
    
    this->timer_width = value;
    this->timer_width_ticks = pow(2,this->timer_width);
    
    // If this is an attached servo, clean up
    if (this->attached())
    {
        // detach, setup and attach again to reflect new timer width
    	//pwm.detachPin(this->pinNumber);
      ledcDetachPin(this->pinNumber);
      
      ledcSetup(this->channel, REFRESH_CPS, this->timer_width);
      ledcAttachPin(this->pinNumber, this->channel);
    }        
}

int Servo::readTimerWidth()
{
    return (this->timer_width);
}

int Servo::usToTicks(int usec)
{
    return (int)((float)usec / ((float)REFRESH_USEC / (float)this->timer_width_ticks)*(((float)REFRESH_CPS)/50.0));
}

int Servo::ticksToUs(int ticks)
{
    return (int)((float)ticks * ((float)REFRESH_USEC / (float)this->timer_width_ticks)/(((float)REFRESH_CPS)/50.0));
}

 
