#include "StateSettings.h"
#include "HardwareManager.h"

static int cursorIndex = 0;
const int maxOptions = 1; // Only "Back" for now

void updateStateSettings() {
  // Read Encoder
  int enc = getEncoderDelta();
  if (enc != 0) {
    cursorIndex += enc;
    // Bound check
    if (cursorIndex < 0) cursorIndex = 0;
    if (cursorIndex >= maxOptions) cursorIndex = maxOptions - 1;
  }

  // Handle Input
  if (isButtonPressed()) {
    if (cursorIndex == 0) {
      currentAppState = APP_CLOCK; // Back to clock
      return;
    }
  }

  // Draw
  display.fillScreen(ST77XX_BLACK);
  display.setTextSize(1);
  display.setTextColor(ST77XX_WHITE);
  
  display.setCursor(0, 0);
  display.println(F("--- SETTINGS ---"));
  
  display.setCursor(10, 30);
  if (cursorIndex == 0) display.print(F("> ")); else display.print(F("  "));
  display.print(F("Back"));

  // Future options can be drawn here at y=50, y=70, etc.
}
