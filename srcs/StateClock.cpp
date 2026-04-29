#include "../include/StateClock.h"
#include "../include/HardwareManager.h"

void updateStateClock() {
    // Check transitions first
    if (isButtonPressed()) {
        currentAppState = AppState::APP_SETTINGS;
        return;
    }

    if (isTiltedRight()) {
        // Requires a small delay or debounce so we don't accidentally rapidly switch
        delay(200);
        currentAppState = AppState::APP_GAME_MENU;
        return;
    }

    // 'now' is already updated in updateHardware()

    // Rendering
    display.fillScreen(ST77XX_BLACK);

    // Draw Time
    display.setTextSize(3);
    display.setTextColor(ST77XX_WHITE);
    display.setCursor(10, 50);

    if (now.hour() < 10)
        display.print("0");
    display.print(now.hour());
    display.print(":");
    if (now.minute() < 10)
        display.print("0");
    display.print(now.minute());

    // Draw Date slightly smaller below
    display.setTextSize(1);
    display.setCursor(25, 90);
    display.print(now.year());
    display.print("/");
    display.print(now.month());
    display.print("/");
    display.print(now.day());
}
