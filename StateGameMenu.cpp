#include "StateGameMenu.h"
#include "HardwareManager.h"

static int gameCursor = 0;
const int maxGames = 1; // Only 2048

void updateStateGameMenu() {
  // Handle transitions
  if (isTiltedLeft()) {
    delay(200);
    currentAppState = APP_CLOCK;
    return;
  }

  // Encoder Navigation
  int enc = getEncoderDelta();
  if (enc != 0) {
    gameCursor += enc;
    if (gameCursor < 0) gameCursor = 0;
    if (gameCursor >= maxGames) gameCursor = maxGames - 1;
  }

  // Select game
  if (isButtonPressed()) {
    if (gameCursor == 0) {
      currentAppState = APP_CALIBRATE_2048;
      return;
    }
  }

  // Render
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("--- GAME MENU ---"));

  display.setCursor(10, 25);
  if (gameCursor == 0) display.print(F("> ")); else display.print(F("  "));
  display.print(F("2048"));

  display.display();
}
