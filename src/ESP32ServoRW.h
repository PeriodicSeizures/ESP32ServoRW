/*
 Originally written by John K. Bennett (2017)

 Rewritten for flexibility, functionality, and simplicity by crazicrafter1/PeriodicSeizures (2020)
 */

#ifndef ESP32_ServoRW_h
#define ESP32_ServoRW_h
//Enforce only using PWM pins on the ESP32
#define ENFORCE_PINS
// Default Arduino Servo.h
#define DEFAULT_uS_LOW 544
#define DEFAULT_uS_HIGH 2400

// Values for TowerPro MG995 large servos (and many other hobbyist servos)
//#define DEFAULT_uS_LOW 1000        // 1000us
//#define DEFAULT_uS_HIGH 2000      // 2000us

// Values for TowerPro SG90 small servos
//#define DEFAULT_uS_LOW 400
//#define DEFAULT_uS_HIGH 2400

#define DEFAULT_TIMER_WIDTH 16
#define DEFAULT_TIMER_WIDTH_TICKS 65536

#define ESP32_Servo_VERSION           1     // software version of this library

#define MIN_PULSE_WIDTH       500     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2500     // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH  1500     // default pulse width when servo is attached
#define DEFAULT_PULSE_WIDTH_TICKS 4825
//#define REFRESH_CPS            50
#define REFRESH_USEC         20000

//#define MAX_SERVOS              16     // no. of PWM channels in ESP32

/*
 * This group/channel/timmer mapping is for information only;
 * the details are handled by lower-level code
 *
 * LEDC Chan to Group/Channel/Timer Mapping
 ** ledc: 0  => Group: 0, Channel: 0, Timer: 0
 ** ledc: 1  => Group: 0, Channel: 1, Timer: 0
 ** ledc: 2  => Group: 0, Channel: 2, Timer: 1
 ** ledc: 3  => Group: 0, Channel: 3, Timer: 1
 ** ledc: 4  => Group: 0, Channel: 4, Timer: 2
 ** ledc: 5  => Group: 0, Channel: 5, Timer: 2
 ** ledc: 6  => Group: 0, Channel: 6, Timer: 3
 ** ledc: 7  => Group: 0, Channel: 7, Timer: 3
 ** ledc: 8  => Group: 1, Channel: 0, Timer: 0
 ** ledc: 9  => Group: 1, Channel: 1, Timer: 0
 ** ledc: 10 => Group: 1, Channel: 2, Timer: 1
 ** ledc: 11 => Group: 1, Channel: 3, Timer: 1
 ** ledc: 12 => Group: 1, Channel: 4, Timer: 2
 ** ledc: 13 => Group: 1, Channel: 5, Timer: 2
 ** ledc: 14 => Group: 1, Channel: 6, Timer: 3
 ** ledc: 15 => Group: 1, Channel: 7, Timer: 3
 */

class Servo {

public:
	Servo(int channel);
	// Arduino Servo Library calls
	int attach(int pin); // attach the given pin to the next free channel, returns channel number or 0 if failure
	int attach(int pin, int min, int max); // as above but also sets min and max values for writes.
	void detach();
	void write(int value); // if value is < MIN_PULSE_WIDTH its treated as an angle, otherwise as pulse width in microseconds
	void writeMicroseconds(int value);     // Write pulse width in microseconds
	int read(); // returns current pulse width as an angle between 0 and 180 degrees
	int readMicroseconds(); // returns current pulse width in microseconds for this servo
	bool attached(); // return true if this servo is attached, otherwise false

	// ESP32 only functions
	void setTimerWidth(int value);     // set the PWM timer width (ESP32 ONLY)
	int readTimerWidth();              // get the PWM timer width (ESP32 ONLY)
	void setPeriodHertz(int hertz){
		REFRESH_CPS=hertz;
		setTimerWidth(this->timer_width);
	}
private:
	int usToTicks(int usec);
	int ticksToUs(int ticks);

	int min = DEFAULT_uS_LOW;           // minimum pulse width for this servo
	int max = DEFAULT_uS_HIGH;            // maximum pulse width for this servo
	int pinNumber = 0;                      // GPIO pin assigned to this channel
	int timer_width = DEFAULT_TIMER_WIDTH; // ESP32 allows variable width PWM timers
	int ticks = DEFAULT_PULSE_WIDTH_TICKS; // current pulse width on this channel
	int timer_width_ticks = DEFAULT_TIMER_WIDTH_TICKS; // no. of ticks at rollover; varies with width
	int REFRESH_CPS = 50;
  int channel;
  bool isAttached;
};

#endif
