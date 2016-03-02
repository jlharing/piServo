 /***************************************************  
  This is a library for our Adafruit 16-channel PWM & Servo driver 
 
  Pick one up today in the adafruit shop! 
  ------> http://www.adafruit.com/products/815 
  
  Written by Limor Fried/Ladyada for Adafruit Industries.   
  BSD license, all text above must be included in any redistribution 
 ****************************************************
 This code is based off of the original Adafruit Arduino
 code and reworked using wiringPi.
 
 There are also added functions.
 ****************************************************/ 

#include <iostream>
#include <stdio.h>
#include <math.h>

#include "piServo.h"
#include "wiringPiI2C.h"
#include "wiringPi.h"
 
using namespace std;

int wp = 0;

// Set to true to print some debug messages, or false to disable them. 
# define ENABLE_DEBUG_OUTPUT false
 
piServo::piServo(void) { 
	piServo(0x40);
} 
piServo::piServo(int addr) { 
	_i2caddr = addr; 
} 

int piServo::begin(void)
{
	return begin(0x40, 159, 456);
}
int piServo::begin(int devID)
{
	return begin(devID, 159, 459);
}
int piServo::begin(int devID, int min, int max) { 
	int i;
	
	if (devID < 0x40 || devID > 0x46)
	{
		cout << "Device address out of range" << endl;
		return -1;
	}
	_i2caddr = devID;
	_servomin = min;
	_servomax = max;
	for (i = 0; i < 16; i++)
	{
		_smin[i] = min;
		_smax[i] = max;
	}
	wp = wiringPiI2CSetup(devID);
	reset();
	return wp;
} 
 
void piServo::reset(void) { 
	write8(PCA9685_MODE1, 0x0); 
} 
 
void piServo::setPWMFreq(float freq) {
	_freq = freq;
	freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11). 
	float prescaleval = 25000000; 
	prescaleval /= 4096; 
	prescaleval /= freq; 
	prescaleval -= 1; 
	if (ENABLE_DEBUG_OUTPUT) { 
		cout << "Estimated pre-scale: " << prescaleval << endl; 
	} 
	int prescale = floor(prescaleval + 0.5); 
	
	if (ENABLE_DEBUG_OUTPUT) { 
		cout << "Final pre-scale: " << prescale << endl; 
	}    
	int oldmode = read8(PCA9685_MODE1); 
	int newmode = (oldmode & 0x7F) | 0x10; // sleep 
	write8(PCA9685_MODE1, newmode); // go to sleep 
	write8(PCA9685_PRESCALE, prescale); // set the prescaler 
	write8(PCA9685_MODE1, oldmode); 
	delay(5); 
	write8(PCA9685_MODE1, oldmode | 0xa1);  //  This sets the MODE1 register to turn on auto increment. 
	                                        // This is why the beginTransmission below was not working. 
} 
 
void piServo::setPWM(int num, int on, int off) { 
	int reg;
	
	reg = 	LED0_ON_L + 4*num; 

	wiringPiI2CWriteReg8(wp, reg, on);
	reg++;
	wiringPiI2CWriteReg8(wp, reg, on >> 8);
	reg++;
	wiringPiI2CWriteReg8(wp, reg, off);
	reg++;
	wiringPiI2CWriteReg8(wp, reg, off >> 8);
} 
 
 
// Sets pin without having to deal with on/off tick placement and properly handles 
// a zero value as completely off.  Optional invert parameter supports inverting 
// the pulse for sinking to ground.  Val should be a value from 0 to 4095 inclusive. 
void piServo::setPin(int num, int val, bool invert) 
{ 
	// Force between min and max
	if (val < _smin[num])
		val = _smin[num];
	if (val > _smax[num])
		val =  _smax[num];
	
	// Clamp value between 0 and 4095 inclusive. 
	if (val > 4095)
		val = 4095;
	
	if (invert) { 
		if (val == 0) { 
		   // Special value for signal fully on. 
			setPWM(num, 4096, 0); 
		} 
		else if (val == 4095) { 
			// Special value for signal fully off. 
			setPWM(num, 0, 4096); 
		} 
		else { 
			setPWM(num, 0, 4095 - val); 
		} 
	} 
	else { 
		if (val == 4095) { 
			// Special value for signal fully on. 
			setPWM(num, 4096, 0); 
		} 
		else if (val == 0) { 
			// Special value for signal fully off. 
			setPWM(num, 0, 4096); 
		} 
		else { 
			setPWM(num, 0, val); 
		} 
	} 
} 
void piServo::setAngle(int num, int val)
{
	int pwm;
	int steps;
	int nintydegree;
	float onedegree;
	
	val = val * -1;
	steps = _smax[num] - _smin[num];
	onedegree = steps / 180;
	nintydegree = _smin[num] + ((_smax[num] - _smin[num]) / 2);
	pwm = nintydegree + (val * onedegree);
	if (pwm < _smin[num])
		pwm = _smin[num];
	if (pwm > _smax[num])
		pwm = _smax[num];
	
	setPin(num, pwm);
}

void piServo::overidePin(int num, int min, int max)
{
	_smin[num] = min;
	_smax[num] = max;
}

int piServo::read8(int addr) { 
	return wiringPiI2CReadReg8(wp, addr);
} 
 
void piServo::write8(int addr, int d) { 
	wiringPiI2CWriteReg8(wp, addr, d);
} 
