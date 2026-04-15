#include "StateClock.h"
#include "HardwareManager.h"

void updateStateClock() {
  // Check transitions first
  if (isButtonPressed()) {
    currentAppState = APP_SETTINGS;
    return;
  }
  
  if (isTiltedRight()) {
    // Requires a small delay or debounce so we don't accidentally rapidly switch
    delay(200); 
    currentAppState = APP_GAME_MENU;
    return;
  }

  // Update logic
  // Update RTC reading
  myRTC.updateTime();

  // Rendering
  display.clearDisplay();
  
  // Draw Time
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  
  if (myRTC.hours < 10) display.print("0");
  display.print(myRTC.hours);
  display.print(":");
  if (myRTC.minutes < 10) display.print("0");
  display.print(myRTC.minutes);

  // Draw Date slightly smaller below
  display.setTextSize(1);
  display.setCursor(35, 50);
  display.print(myRTC.year);
  display.print("/");
  display.print(myRTC.month);
  display.print("/");
  display.print(myRTC.dayofmonth);
  
  display.display();
}
