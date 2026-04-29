#ifndef GAME2048_H
#define GAME2048_H

#include <Arduino.h>
#include <stdint.h>
#include <stdint.h>

#define MOVE_COOLDOWN 300

uint8_t gameLoop();
enum Game2048State { GSTATE_PLAYING, GSTATE_GAMEOVER };

enum class Direction : uint8_t {
    DIR_NONE = 0x0,
    DIR_UP = 0x1,
    DIR_DOWN = 0x2,
    DIR_LEFT = 0x3,
    DIR_RIGHT = 0x4
};

struct GameStatusRecord {
    // four bit an unit of board record
    // I use pow(2,board's_four_bit)
    // 0000 0000 0000 0000
    // 3項  2項  1項  0項
    uint16_t board[4];
    uint32_t score;
    bool victory;
    uint32_t lastMoveTime;
    bool isTiltedGate;
    Direction dir;
};
// Lifecycle deprecated
void initGame2048();
void updateGame2048();
void resetGame();
#endif
