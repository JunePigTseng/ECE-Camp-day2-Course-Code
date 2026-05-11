#include <Adafruit_GFX.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_ST7735.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <stdint.h>

#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9
#define ENC_CLK 3
#define ENC_DT 4
#define ENC_SW 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

uint8_t pageState = 0;      // 0=menu, 1=2048, 2=accel test
uint8_t menuItem = 0;       // 0=2048, 1=accel test
bool screenNeedsUpdate = true;
bool needInitPage = true;
uint8_t lastClk = HIGH;

void setup() {
  Serial.begin(115200);
  // Initialize the display
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  display.initR(INITR_144GREENTAB);
  display.setRotation(0);
  display.fillScreen(ST77XX_BLACK);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(F("ECE Camp Menu"));
  Serial.println("ECE Camp Menu");

  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);

  if (!accel.begin()) {
    Serial.println(F("ADXL345 init failed"));
    display.println(F("ADXL345 init failed"));
    while (1) {
      delay(100);
    }
  }

  accel.setRange(ADXL345_RANGE_4_G);
  Serial.println(F("ADXL345 initialized"));
}

void loop() {
  uint8_t currentClk = digitalRead(ENC_CLK);
  if (currentClk != lastClk && currentClk == LOW) {
    if (digitalRead(ENC_DT) != currentClk) {
      // 順時針：下一個選項
      menuItem = (menuItem + 1) % 2;
    } else {
      // 逆時針：上一個選項
      menuItem = (menuItem == 0) ? 1 : menuItem - 1;
    }
    screenNeedsUpdate = true;
  }
  lastClk = currentClk;

  if (digitalRead(ENC_SW) == LOW) {
    pageState = menuItem + 1;  // 0->1 (2048), 1->2 (accel test)
    needInitPage = true;
    screenNeedsUpdate = true;
    delay(200);  // debounce
  }

  if (screenNeedsUpdate || (pageState == 2 && needInitPage == false)) {
    switch (pageState) {
      case 0:  // menu
        if (needInitPage) {
          display.fillScreen(ST77XX_BLACK);
          display.setCursor(0, 0);
          display.println(F("Menu"));
          display.println(F(""));
          display.println(F(" 2048 Game"));
          display.println(F(" ADXL Test"));
          needInitPage = false;
        }
        display.setCursor(0, 30 + 30 * menuItem);
        display.print(F(">"));
        Serial.print(F("Selected item: "));
        Serial.println(menuItem);
        break;
      case 1:  // 2048 game
        if (needInitPage) {
          display.fillScreen(ST77XX_BLACK);
          display.setCursor(0, 0);
          display.println(F("2048 Game"));
          display.println(F("(TBD)"));
          needInitPage = false;
        }
        // Game logic here (placeholder)
        break;
      case 2:  // accel test
        if (needInitPage) {
          display.fillScreen(ST77XX_BLACK);
          display.setCursor(0, 0);
          display.println(F("ADXL Test"));
          display.println(F("Roll:"));
          display.println(F("Pitch:"));
          display.println(F("Z:"));
          needInitPage = false;
        }
        sensors_event_t event;
        accel.getEvent(&event);
        display.setCursor(50, 30);
        float roll = atan2(event.acceleration.y, event.acceleration.z) * 180.0 / PI;
        float pitch = atan2(-event.acceleration.x, sqrt(event.acceleration.y * event.acceleration.y + event.acceleration.z * event.acceleration.z)) * 180.0 / PI;
        display.print(F("     "));  // clear previous
        display.setCursor(50, 30);
        display.print(roll, 1);
        display.print(F(" deg"));
        display.setCursor(50, 50);
        display.print(F("     "));
        display.setCursor(50, 50);
        display.print(pitch, 1);
        display.print(F(" deg"));
        display.setCursor(50, 70);
        display.print(F("     "));
        display.setCursor(50, 70);
        display.print(event.acceleration.z, 2);
        display.print(F(" m/s2"));
        break;
    }
    screenNeedsUpdate = false;
  }

  delay(100);
}
