#include "StateCalibrate.h"
#include "HardwareManager.h"

void updateStateCalibrate() {
  if (isButtonPressed()) {
    captureBaseline();
    currentAppState = APP_PLAY_2048;
    return;
  }

  display.fillScreen(ST77XX_BLACK);
  display.setTextSize(1);
  display.setTextColor(ST77XX_WHITE);
  display.setCursor(0, 0);
  display.println(F("CALIBRATION"));
  
  display.setCursor(0, 30);
  display.println(F("Hold device flat"));
  display.println(F("in play position"));
  
  display.setCursor(0, 70);
  display.println(F("Press Btn to start!"));
}
