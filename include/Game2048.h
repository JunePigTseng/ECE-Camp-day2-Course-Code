#ifndef GAME2048_H
#define GAME2048_H

#include "Globals.h"
#include <Arduino.h>

// Lifecycle
void initGame2048();
void updateGame2048();

enum class Direction : uint8_t {
    DIR_NONE = 0x0,
    DIR_UP = 0x1,
    DIR_DOWN = 0x2,
    DIR_LEFT = 0x3,
    DIR_RIGHT = 0x4
};
enum Game2048State { GSTATE_PLAYING, GSTATE_GAMEOVER };
/* struct GameStatusRecord {
     uint16_t board[4][4];
     uint32_t score;
     bool victory;
     uint32_t lastMoveTime;
     bool isTiltedGate;
     Game2048State currentState;
};*/
#endif
