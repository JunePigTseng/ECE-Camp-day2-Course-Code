#include "include/BuddhaBlessing.h"
#include "include/Globals.h"
#include "include/HardwareManager.h"
#include <math.h>

// ================================================================
// Private macros
// ================================================================
#define BB_BG ILI9341_BLACK
#define BB_GOLD 0xFE60u // warm gold (RGB565)
#define BB_RED 0xF800u  // auspicious red
#define BB_HINT 0x7BEFu // grey hint text

// ================================================================
// Blessing text lines – edit these to change what is displayed.
// Each entry is one line; empty string "" draws nothing for that row.
// Use F() macro strings in the draw function to keep them in PROGMEM.
// ================================================================

// ================================================================
// Static helpers
// ================================================================

// Draw decorative lotus pattern using geometric primitives
static void drawLotus(int16_t cx, int16_t cy) {
    // Outer petals (8 ellipses approximated with filled circles)
    for (uint8_t i = 0; i < 8; i++) {
        float angle = i * (3.14159f / 4.0f);
        int16_t px = cx + (int16_t)(22 * cos(angle));
        int16_t py = cy + (int16_t)(22 * sin(angle));
        tft.fillCircle(px, py, 10, BB_GOLD);
    }
    // Inner petals
    for (uint8_t i = 0; i < 8; i++) {
        float angle = i * (3.14159f / 4.0f) + (3.14159f / 8.0f);
        int16_t px = cx + (int16_t)(14 * cos(angle));
        int16_t py = cy + (int16_t)(14 * sin(angle));
        tft.fillCircle(px, py, 7, BB_RED);
    }
    // Center
    tft.fillCircle(cx, cy, 8, BB_GOLD);
    tft.fillCircle(cx, cy, 4, BB_RED);
}

// Draw decorative border
static void drawBorder() {
    // Double-line gold border
    tft.drawRect(4, 4, TFT_W - 8, TFT_H - 8, BB_GOLD);
    tft.drawRect(8, 8, TFT_W - 16, TFT_H - 16, BB_GOLD);
    // Corner dots
    uint16_t corners[][2] = {
        {12, 12}, {TFT_W - 13, 12}, {12, TFT_H - 13}, {TFT_W - 13, TFT_H - 13}};
    for (uint8_t i = 0; i < 4; i++) {
        tft.fillCircle(corners[i][0], corners[i][1], 3, BB_RED);
    }
}

// Draw the full blessing screen
static void drawBlessingScreen() {
    tft.fillScreen(BB_BG);

    // Decorative border
    drawBorder();

    // Top lotus
    drawLotus(TFT_W / 2, 60);

    // ---- Main blessing text ----
    // TODO: Replace placeholder text with desired blessing content.
    //       Edit the lines below. Use tft.print(F("your text"));
    tft.setTextSize(2);
    tft.setTextColor(BB_GOLD, BB_BG);
    tft.setCursor(30, 110);
    tft.print(F("== BLESSING =="));

    // Placeholder area for custom text (lines 1-4)
    tft.setTextSize(2);
    tft.setTextColor(BB_RED, BB_BG);

    // Line 1
    tft.setCursor(20, 150);
    tft.print(F("")); // <-- edit here

    // Line 2
    tft.setCursor(20, 175);
    tft.print(F("")); // <-- edit here

    // Line 3
    tft.setCursor(20, 200);
    tft.print(F("")); // <-- edit here

    // Line 4
    tft.setCursor(20, 225);
    tft.print(F("")); // <-- edit here

    // Bottom lotus
    drawLotus(TFT_W / 2, 265);

    // Exit hint
    tft.setTextSize(1);
    tft.setTextColor(BB_HINT, BB_BG);
    tft.setCursor(40, 305);
    tft.print(F("Press button to exit"));
}

// ================================================================
// loopBuddhaBlessing – public entry (blocking)
// ================================================================
void loopBuddhaBlessing() {
    drawBlessingScreen();

    // Wait for button press to exit
    while (true) {
        uint32_t fs = millis();
        updateHardware();

        if (isButtonPressed()) return;

        FRAME_DELAY(fs);
    }
}
