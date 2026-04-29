#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#ifndef RELATIVE_TILT_THRESHOLD
#define RELATIVE_TILT_THRESHOLD 20
#endif // !RELATIVE_TILT_THRESHOLD
enum class AppState : uint8_t {
    APP_CLOCK = 0x0,
    APP_SETTINGS = 0x1,
    APP_GAME_MENU = 0x2,
    APP_CALIBRATE_2048 = 0x3,
    APP_PLAY_2048 = 0x4
};

extern AppState currentAppState;

#endif
