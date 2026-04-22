#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <cstdint>

// ST7735S TFT SPI Display Pins
#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

extern Adafruit_ST7735 display;
extern Adafruit_MPU6050 mpu;
extern RTC_DS3231 myRTC;
extern DateTime now;

int8_t initHardware();
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
