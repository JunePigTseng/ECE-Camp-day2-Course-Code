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
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define ENC_CLK 3
#define ENC_DT 4
#define ENC_SW 2
#define MPU_ADDR 0x53 // if(sd0 == LOW) {0x53} else {0x1D}  

Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
// Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// uint8_t pageState = 0;      // 0=menu, 1=2048, 2=unused, 3=accel test
// uint8_t menuItem = 0;       // 0=2048, 1=unused, 2=accel test
// bool screenNeedsUpdate = true;
// bool needInitPage = true;
// uint8_t lastClk = HIGH;

void setup() {
  // hardware initialization
  // Serial for debugging
  Serial.begin(115200);
  // Initialize the display
  // 1. 強制讓螢幕 Reset
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  // display.initR(INITR_BLACKTAB);
  display.initR(INITR_144GREENTAB);
  display.setRotation(0);
  display.fillScreen(ST77XX_BLUE);
  display.drawRect(0, 0, 128, 128, ST77XX_YELLOW);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(F("Hello, ECE Camp!"));
  Serial.println("Hello, ECE Camp!");
  // Initialize the encoder pins
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);

  // // Initialize the accelerometer
  // delay(1000);
  // if (!accel.begin()) {
  //   Serial.println(F("Failed to initialize ADXL345!"));
  //   display.println(F("MPU Init Failed!"));
  //   for(;;);
  // }
  // Serial.println(F("ADXL345 initialized successfully!"));
  // accel.setRange(ADXL345_RANGE_4_G);
}

void loop() {
  // --- A. 偵測編碼器旋轉 ---
  // uint8_t currentClk = digitalRead(ENC_CLK);
  // if (currentClk != lastClk && currentClk == LOW) { // 偵測下降沿
  //   if (digitalRead(ENC_DT) != currentClk) {
  //     // 順時針：切換選單項
  //     menuItem = (menuItem + 1) % 3;
  //   } else {
  //     // 逆時針
  //     menuItem = (menuItem == 0) ? 2 : menuItem - 1;
  //   }
  //   screenNeedsUpdate = true;
  // }
  // lastClk = currentClk; // 更新lastClk為currentClk

  // bool shouldRender = screenNeedsUpdate || pageState == 3;
  // if (shouldRender) {
  //   switch (pageState) {
  //   case 0:
  //     if (needInitPage){
  //       display.fillScreen(ST77XX_BLACK);
  //       display.setCursor(0, 0);
  //       display.println(F("Menu\n"));
  //       display.println(F(" 2048 Game"));
  //       display.println(F(" Unused"));
  //     }
  //     display.println(F(" AccelTest"));
  //     display.setCursor(0, 30 + 30 * menuItem);
  //     display.print(F(">"));
  //     break;
  //   case 1:
  //     // game 2048
  //     display.fillScreen(ST77XX_BLACK);
  //     display.setCursor(0, 0);
  //     display.println(F("2048 Game (TBD)"));
  //     break;
  //   case 2:
  //     // game unused
  //     display.fillScreen(ST77XX_BLACK);
  //     display.setCursor(0, 0);
  //     display.println(F("Unused Game"));
  //     break;
  //   case 3:
  //     if (needInitPage) {
  //       display.fillScreen(ST77XX_BLACK);
  //       display.setCursor(0, 0);
  //       display.println(F("Accel Test"));
  //       display.println(F("Roll :"));
  //       display.println(F("Pitch:"));
  //       display.println(F("Z    :"));
  //       needInitPage = false;
  //     }
  //     sensors_event_t event;
  //     accel.getEvent(&event);
  //     display.setCursor(12, 30);

  //     float roll = atan2(event.acceleration.y, event.acceleration.z) * 180.0 / PI;
  //     float pitch = atan2(-event.acceleration.x, sqrt(event.acceleration.y * event.acceleration.y + event.acceleration.z * event.acceleration.z)) * 180.0 / PI;

  //     display.print(F("Roll: ")); display.print(roll, 1); display.println(F(" deg"));
  //     display.print(F("Pitch: ")); display.print(pitch, 1); display.println(F(" deg"));
  //     display.print(F("Z: ")); display.print(event.acceleration.z, 2); display.println(F(" m/s^2"));
  //     break;
  //   }

  //   screenNeedsUpdate = false;
  // }
}