#include "../include/HardwareManager.h"
#include <cstdint>

// Define Pins
#define ENC_CLK 3
#define ENC_DT 4
#define ENC_SW 2

Adafruit_ST7735 display(TFT_CS, TFT_DC, TFT_RST);
Adafruit_MPU6050 mpu;

// Init RTC (I2C, uses Wire: SDA=A4, SCL=A5)
RTC_DS3231 myRTC;
DateTime now;

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

// retrun
//  0 -> success
//  1 -> RTC Failed
//  2 -> MPU Failed
int8_t initHardware() {
    Serial.begin(115200);

    // Init SPI TFT Display (ST7735S)
    display.initR(INITR_BLACKTAB); // Use INITR_BLACKTAB for most ST7735S modules
    display.setRotation(0);        // Adjust rotation as needed (0-3)
    display.fillScreen(ST77XX_BLACK);
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(F("Connecting MPU..."));

    // Init I2C RTC (DS3231)
    if (!myRTC.begin()) return 1;
    // If RTC lost power, set the time to compile time:
    if (myRTC.lostPower()) {
        myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    // If you need to manually set time, uncomment:
    // myRTC.adjust(DateTime(2026, 1, 10, 23, 59, 0));

    // Init MPU6050
    if (!mpu.begin()) return 2;

    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    pinMode(ENC_CLK, INPUT_PULLUP);
    pinMode(ENC_DT, INPUT_PULLUP);
    pinMode(ENC_SW, INPUT_PULLUP);
    lastClk = digitalRead(ENC_CLK);
}

void updateHardware() {
    // Update RTC time once per loop
    now = myRTC.now();

    // Simple Polling Encoder Read
    int currentClk = digitalRead(ENC_CLK);
    if (currentClk != lastClk && currentClk == 1) {
        (digitalRead(ENC_DT) ^ currentClk)? encoderDelta++: encoderDelta--;
        
    }
    lastClk = currentClk;

    // Simple Polling Button Debounce
    bool currentBtn = digitalRead(ENC_SW);
    if (currentBtn ^ lastBtnState) lastDebounceTime = millis();

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

float getBaseX() {
    return baseX;
}
float getBaseY() {
    return baseY;
}
float getBaseZ() {
    return baseZ;
}
