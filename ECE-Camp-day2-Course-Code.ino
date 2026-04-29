#include "include/Globals.h"
#include "include/HardwareManager.h"
#include "include/StateClock.h"
#include "include/StateSettings.h"
#include "include/StateGameMenu.h"
#include "include/StateCalibrate.h"
#include "include/Game2048.h"

// Define Global State
AppState currentAppState = AppState::APP_CLOCK;
AppState lastAppState = AppState::APP_CLOCK;

void setup() {
  switch (initHardware()){
    case 1:
      display.println(F("RTC Failed"));
      for(;;);
    case 2:
      display.println(F("MPU Failed"));
      for(;;);
    default:
      display.println(F("Hardware init Success"));
  }
  // Need to call init for the current app state if needed
  // Right now, Clock doesn't need an explicit init() call every time.
}

void loop() {
  // Always update hardware inputs exactly once per loop cycle
  updateHardware();

  // If we just entered a new state, fire any 1-time init routines
  // currentAppState != lastAppState?
  if (static_cast<uint8_t>(currentAppState) ^ static_cast<uint8_t>(lastAppState)) {
    if (!static_cast<uint8_t>(currentAppState) ^ static_cast<uint8_t>(AppState::APP_PLAY_2048)) {
      initGame2048();
    }
    // Could add init routines for others here
    lastAppState = currentAppState;
  }

  // State Machine Router
  switch(currentAppState) {
    case AppState::APP_CLOCK:
      updateStateClock();
      break;
    case AppState::APP_SETTINGS:
      updateStateSettings();
      break;
    case AppState::APP_GAME_MENU:
      updateStateGameMenu();
      break;
    case AppState::APP_CALIBRATE_2048:
      updateStateCalibrate();
      break;
    case AppState::APP_PLAY_2048:
      updateGame2048();
      break;
  }
}
