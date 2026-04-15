#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <virtuabotixRTC.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

extern Adafruit_SSD1306 display;
extern Adafruit_MPU6050 mpu;
extern virtuabotixRTC myRTC;

void initHardware();
void updateHardware();

// Rotary Encoder logic
int getEncoderDelta();
bool isButtonPressed();

// MPU Tilt Gestures
bool isTiltedRight(); // Roll Right by ~90 degrees
bool isTiltedLeft();  // Back to flat / Roll Left

// Calibrated Game Baseline
void captureBaseline();
float getBaseX();
float getBaseY();
float getBaseZ();

#endif
