#include "../include/StateGameMenu.h"
#include "../include/HardwareManager.h"

static int gameCursor = 0;
const int maxGames = 1; // Only 2048

void updateStateGameMenu() {
    // Handle transitions
    if (isTiltedLeft()) {
        delay(200);
        currentAppState = AppState::APP_CLOCK;
        return;
    }

    // Encoder Navigation
    int enc = getEncoderDelta();
    if (enc != 0) {
        gameCursor += enc;
        if (gameCursor < 0)
            gameCursor = 0;
        if (gameCursor >= maxGames)
            gameCursor = maxGames - 1;
    }

    // Select game
    if (isButtonPressed()) {
        if (gameCursor == 0) {
            currentAppState = AppState::APP_CALIBRATE_2048;
            return;
        }
    }

    // Render
    display.fillScreen(ST77XX_BLACK);
    display.setTextSize(1);
    display.setTextColor(ST77XX_WHITE);
    display.setCursor(0, 0);
    display.println(F("--- GAME MENU ---"));

    display.setCursor(10, 35);
    if (gameCursor == 0)
        display.print(F("> "));
    else
        display.print(F("  "));
    display.print(F("2048"));
}
