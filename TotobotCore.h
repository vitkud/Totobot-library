/*
	TotobotCore.h - Core library for Totobot - description
	Copyright (c) 2019 Vitaliy.  All right reserved.
*/

#ifndef TotobotCore_h
#define TotobotCore_h

#include "Arduino.h"

#include "AFMotor.h"
#include "TimerOne.h"
#include "FastLED.h"

#define BRIGHTNESS 3 // (0-255)
#define CURRENT_LIMIT 600
#define WIDTH 4
#define HEIGHT 4
#define EYE_LEDS 16 // WIDTH * HEIGHT
#define NUM_LEDS 32 // EYE_LEDS * 2
#define LED_PIN A0

#define SNOW_DENSE 10

class Totobot {
public:
	Totobot();

	void init();

	void setCorrection(int value);
	void moveForward(int duration, byte speed);
	void moveBackward(int duration, byte speed);
	void turnLeft(int duration, byte speed);
	void turnRight(int duration, byte speed);
	void runMotor(int port, int speed);

	void setEyeEffect(int eye, int effect);

private:
	static void timer();
	int corr = 0;
	AF_DCMotor motor[2] = {1, 2};

	volatile boolean eyeLoadingFlag[2];
	volatile int eyeEffects[2];
	static void updateEffect(int eye, int effect, boolean loadingFlag);
};

extern Totobot totobot;

#endif
