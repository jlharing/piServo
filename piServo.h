#pragma once
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

# define PCA9685_SUBADR1 0x2
# define PCA9685_SUBADR2 0x3
# define PCA9685_SUBADR3 0x4

# define PCA9685_MODE1 0x0
# define PCA9685_PRESCALE 0xFE

# define LED0_ON_L 0x6
# define LED0_ON_H 0x7
# define LED0_OFF_L 0x8
# define LED0_OFF_H 0x9

# define ALLLED_ON_L 0xFA
# define ALLLED_ON_H 0xFB
# define ALLLED_OFF_L 0xFC
# define ALLLED_OFF_H 0xFD

class piServo {
public:
	piServo(void);
	piServo(int addr);
	int begin(void);
	int begin(int);
	int begin(int, int, int);
	void reset(void);
	void setPWMFreq(float freq);
	void setPWM(int num, int on, int off);
	void setPin(int num, int val, bool invert = false);
	void setAngle(int num, int val);
	void overidePin(int num, int min, int max);
private:
	int _i2caddr;
	float _freq = 50;
	// Servo Min & Max @ 50mhz
	int _servomin = 159;
	int _servomax = 456;
	int _smin[16];
	int _smax[16];

	int read8(int addr);
	void write8(int addr, int d);
};