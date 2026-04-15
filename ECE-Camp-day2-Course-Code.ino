#include "Globals.h"
#include "HardwareManager.h"
#include "StateClock.h"
#include "StateSettings.h"
#include "StateGameMenu.h"
#include "StateCalibrate.h"
#include "Game2048.h"

// Define Global State
AppState currentAppState = APP_CLOCK;
AppState lastAppState = APP_CLOCK;

void setup() {
  initHardware();
  
  // Need to call init for the current app state if needed
  // Right now, Clock doesn't need an explicit init() call every time.
}

void loop() {
  // Always update hardware inputs exactly once per loop cycle
  updateHardware();

  // If we just entered a new state, fire any 1-time init routines
  if (currentAppState != lastAppState) {
    if (currentAppState == APP_PLAY_2048) {
      initGame2048();
    }
    // Could add init routines for others here
    lastAppState = currentAppState;
  }

  // State Machine Router
  switch(currentAppState) {
    case APP_CLOCK:
      updateStateClock();
      break;
    case APP_SETTINGS:
      updateStateSettings();
      break;
    case APP_GAME_MENU:
      updateStateGameMenu();
      break;
    case APP_CALIBRATE_2048:
      updateStateCalibrate();
      break;
    case APP_PLAY_2048:
      updateGame2048();
      break;
  }
}
