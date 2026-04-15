#include "HardwareManager.h"

// Define Pins
#define ENC_CLK 3
#define ENC_DT 4
#define ENC_SW 2

#define RTC_CLK 5
#define RTC_DAT 6
#define RTC_RST 7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;

// Init RTC
virtuabotixRTC myRTC(RTC_CLK, RTC_DAT, RTC_RST);

// Internal Input State
static int encoderDelta = 0;
static int lastClk = HIGH;
static bool lastBtnState = HIGH;
static bool btnPressed = false;
static uint32_t lastDebounceTime = 0;

// Baseline Calibration
static float baseX = 0;
static float baseY = 0;
static float baseZ = 0;

void initHardware() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Connecting MPU..."));
  display.display();

  if (!mpu.begin()) {
    display.println(F("MPU Failed!"));
    display.display();
    while (1) { delay(10); }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  lastClk = digitalRead(ENC_CLK);

  // If you need to set initial time, uncomment this line: 
  // myRTC.setDS1302Time(00, 59, 23, 6, 10, 1, 2026); 
  // seconds, minutes, hours, day of week, day, month, year
}

void updateHardware() {
  // Simple Polling Encoder Read
  int currentClk = digitalRead(ENC_CLK);
  if (currentClk != lastClk && currentClk == 1) {
    if (digitalRead(ENC_DT) != currentClk) {
      encoderDelta++;
    } else {
      encoderDelta--;
    }
  }
  lastClk = currentClk;

  // Simple Polling Button Debounce
  bool currentBtn = digitalRead(ENC_SW);
  if (currentBtn != lastBtnState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > 50) {
    // Only trigger on release or press. Setting it to trigger on press (LOW):
    if (currentBtn == LOW && lastBtnState == HIGH) {
      btnPressed = true;
    }
  }
  lastBtnState = currentBtn;
}

int getEncoderDelta() {
  int delta = encoderDelta;
  encoderDelta = 0; // Consume
  return delta;
}

bool isButtonPressed() {
  if (btnPressed) {
    btnPressed = false; // Consume
    return true;
  }
  return false;
}

bool isTiltedRight() {
  // When watch is physically rotated 90 deg right around Z
  // This depends on the physical orientation of the MPU.
  // Generally, X or Y will jump to > 6.0 or < -6.0.
  // assuming normal flat is Z=9.8, right tilt means X jumps.
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  if (a.acceleration.x < -6.0) return true;
  return false;
}

bool isTiltedLeft() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // If moving back to flat, X should be near 0 and Z should be near 9.8.
  // Wait, if "left tilt" means returning to initial holding:
  if (abs(a.acceleration.x) < 3.0 && a.acceleration.z > 6.0) return true;
  return false;
}

void captureBaseline() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  baseX = a.acceleration.x;
  baseY = a.acceleration.y;
  baseZ = a.acceleration.z;
}

float getBaseX() { return baseX; }
float getBaseY() { return baseY; }
float getBaseZ() { return baseZ; }
