#include "StateCalibrate.h"
#include "HardwareManager.h"

void updateStateCalibrate() {
  if (isButtonPressed()) {
    captureBaseline();
    currentAppState = APP_PLAY_2048;
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("CALIBRATION"));
  
  display.setCursor(0, 20);
  display.println(F("Hold device flat"));
  display.println(F("in play position"));
  
  display.setCursor(0, 50);
  display.println(F("Press Btn to start!"));
  display.display();
}
