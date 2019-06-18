/*
	TotobotCore.cpp - Core library for Totobot - implementation
	Copyright (c) 2019 Vitaliy.  All right reserved.
	Used effects from https://github.com/AlexGyver/GyverMatrixBT/
*/

#include "TotobotCore.h"

Totobot totobot;

CRGB leds[NUM_LEDS];

void Totobot::timer() {
	for (int i = 0; i < 2; ++i) {
		updateEffect(i, totobot.eyeEffects[i], totobot.eyeLoadingFlag[i]);
		totobot.eyeLoadingFlag[i] = false;
	}
	FastLED.show(); 
}

Totobot::Totobot() {
}

void Totobot::init() {
	motor[0].run(RELEASE);
	motor[1].run(RELEASE);

	FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(BRIGHTNESS);
	if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
	FastLED.clear();
	FastLED.show();

	Timer1.initialize(100000);
	Timer1.attachInterrupt(timer);
}

void Totobot::setCorrection(int value) {
	corr = value;
}
void Totobot::moveForward(int duration, byte speed) {
	motor[0].run(FORWARD);
	motor[0].setSpeed(speed-(corr>0?corr:0));
	motor[1].run(FORWARD);
 	motor[1].setSpeed(speed+(corr<0?corr:0));
 	delay(duration*1000);
 	motor[0].run(RELEASE);
 	motor[1].run(RELEASE);
}
void Totobot::moveBackward(int duration, byte speed) {
	motor[0].run(BACKWARD);
	motor[0].setSpeed(speed-(corr>0?corr:0));
	motor[1].run(BACKWARD);
	motor[1].setSpeed(speed+(corr<0?corr:0));
	delay(duration*1000);
	motor[0].run(RELEASE);
	motor[1].run(RELEASE);
}
void Totobot::turnLeft(int duration, byte speed) {
	motor[0].run(BACKWARD);
	motor[0].setSpeed(speed-(corr>0?corr:0));
	motor[1].run(FORWARD);
	motor[1].setSpeed(speed+(corr<0?corr:0));
	delay(duration*1000);
	motor[0].run(RELEASE);
	motor[1].run(RELEASE);
}
void Totobot::turnRight(int duration, byte speed) {
	motor[0].run(FORWARD);
	motor[0].setSpeed(speed-(corr>0?corr:0));
	motor[1].run(BACKWARD);
	motor[1].setSpeed(speed+(corr<0?corr:0));
	delay(duration*1000);
	motor[0].run(RELEASE);
	motor[1].run(RELEASE);
}
void Totobot::runMotor(int port, int speed) {
	motor[port].setSpeed(speed >= 0 ? speed : -speed);
	motor[port].run(speed > 0 ? FORWARD : speed < 0 ? BACKWARD : RELEASE);
}

void Totobot::setEyeEffect(int eye, int effect) {
	if (eye < 0 || eye >=2) return;
	eyeEffects[eye] = effect;
	eyeLoadingFlag[eye] = true;
}

#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y (HEIGHT - y - 1)

uint16_t getPixelNumber(int eye, int8_t x, int8_t y) {
  if (THIS_Y % 2 == 0) {
    return (THIS_Y * _WIDTH + THIS_X) + EYE_LEDS * eye;
  } else {
    return (THIS_Y * _WIDTH + _WIDTH - THIS_X - 1) + EYE_LEDS * eye;
  }
}

void drawPixelXY(int eye, int8_t x, int8_t y, CRGB color) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
  leds[getPixelNumber(eye, x, y)] = color;
}

uint32_t getPixColor(int pixelNumber) {
  if (pixelNumber < 0 || pixelNumber >= EYE_LEDS) return 0;
  return (((uint32_t)leds[pixelNumber].r << 16) | ((long)leds[pixelNumber].g << 8 ) | (long)leds[pixelNumber].b); // XXX uint32_t, long, long
}

uint32_t getPixColorXY(int eye, int8_t x, int8_t y) {
  return getPixColor(getPixelNumber(eye, x, y));
}

void noneRoutine(int eye, boolean loadingFlag) {
	if (loadingFlag) {
		for (int i = eye; i < EYE_LEDS; ++i)
			leds[i + EYE_LEDS * eye] = 0;
	}
}

void snowRoutine(int eye, boolean loadingFlag) {
	for (byte x = 0; x < WIDTH; x++) {
		for (byte y = 0; y < HEIGHT - 1; y++) {
			drawPixelXY(eye, x, y, getPixColorXY(eye, x, y + 1));
		}
	}

	for (byte x = 0; x < WIDTH; x++) {
		if (getPixColorXY(eye, x, HEIGHT - 2) == 0 && (random(0, SNOW_DENSE) == 0))
			drawPixelXY(eye, x, HEIGHT - 1, 0xE0FFFF - 0x101010 * random(0, 4));
		else
			drawPixelXY(eye, x, HEIGHT - 1, 0x000000);
	}
}

void matrixRoutine(int eye, boolean loadingFlag) {
	if (loadingFlag)
		noneRoutine(eye, loadingFlag);

	for (byte x = 0; x < WIDTH; x++) {
		uint32_t thisColor = getPixColorXY(eye, x, HEIGHT - 1);
		if (thisColor == 0)
			drawPixelXY(eye, x, HEIGHT - 1, 0x00FF00 * (random(0, 10) == 0));
		else if (thisColor < 0x002000)
			drawPixelXY(eye, x, HEIGHT - 1, 0);
		else
			drawPixelXY(eye, x, HEIGHT - 1, thisColor - 0x002000);
	}

	for (byte x = 0; x < WIDTH; x++) {
		for (byte y = 0; y < HEIGHT - 1; y++) {
			drawPixelXY(eye, x, y, getPixColorXY(eye, x, y + 1));
		}
	}
}

void Totobot::updateEffect(int eye, int effect, boolean loadingFlag) {
	switch (effect) {
	case 0: noneRoutine(eye, loadingFlag); break;
	case 2: snowRoutine(eye, loadingFlag); break;
	case 7: matrixRoutine(eye, loadingFlag); break;
	default:
		break;
	}
}
