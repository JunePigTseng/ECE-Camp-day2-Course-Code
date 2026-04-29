#include "../include/Game2048.h"
#include "../include/Globals.h"
#include "../include/HardwareManager.h"
#include <cstdint>

static GameStatusRecord gameDataRecord;

static void initGame();
static uint8_t checkGameAliveAndAddRandomTile(uint16_t *);

static void initGame() {
    gameDataRecord.score = 0;
    gameDataRecord.victory = false;
    gameDataRecord.lastMoveTime = 0;
    gameDataRecord.isTiltedGate = false;
    gameDataRecord.dir = Direction::DIR_NONE;
    memset(gameDataRecord.board, 0, sizeof(gameDataRecord.board));
    checkGameAliveAndAddRandomTile(gameDataRecord.board);
}

// return  bit
// 1 -> Lose / vectory 1/0
// 2 -> userInterrupt
uint8_t gameLoop() {
    initGame();
    for (; (!isButtonPressed()) && ((millis() - gameDataRecord.lastMoveTime > MOVE_COOLDOWN));) {
        if (checkGameAliveAndAddRandomTile(gameDataRecord.board) & 0b100) {
            goto GameOver;
        }
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        float dX = a.acceleration.x - getBaseX(), dY = a.acceleration.y - getBaseY();
        if (dX > RELATIVE_TILT_THRESHOLD)
            gameDataRecord.dir = Direction::DIR_LEFT;
        else if (dX < -RELATIVE_TILT_THRESHOLD)
            gameDataRecord.dir = Direction::DIR_RIGHT;
        else if (dY > RELATIVE_TILT_THRESHOLD)
            gameDataRecord.dir = Direction::DIR_DOWN;
        else if (dY < -RELATIVE_TILT_THRESHOLD)
            gameDataRecord.dir = Direction::DIR_UP;

        // 轉換型態成uint8_t 後去判斷是否相同 只用 1*CLK
        if (static_cast<uint8_t>(gameDataRecord.dir) ^ static_cast<uint8_t>(Direction::DIR_NONE)) {
            if (!gameDataRecord.isTiltedGate) {
                bool moved = false;
                switch (gameDataRecord.dir) {
                case Direction::DIR_LEFT:
                    moved = moveLeft();
                    break;
                case Direction::DIR_RIGHT:
                    moved = moveRight();
                    break;
                case Direction::DIR_UP:
                    moved = moveUp();
                    break;
                case Direction::DIR_DOWN:
                    moved = moveDown();
                    break;
                default:
                    break;
                }

                if (moved) {
                    checkGameAliveAndAddRandomTile(gameDataRecord.board);
                    if (isGameOverLocal()) {
                        currentState = GSTATE_GAMEOVER;
                    }
                }
                gameDataRecord.isTiltedGate = true;
                gameDataRecord.lastMoveTime = millis();
            }
        } else continue;

        drawBoard();
    }
    return 2;
GameOver:
    return 1;
}

// 我寫了三小
// return_data bit[n] ->
// 1 -> row is full
// 2 -> col is full
// 3 -> 1/0 Dead/Alive
static uint8_t checkGameAliveAndAddRandomTile(uint16_t *const board) {
    // 如果 board[row][col] 有一個 ceil 是空的則直接新增 tile
    if (!((*(board + 0) & 0x000F) && ((*board + 0) & 0x00F0) && (*(board + 0) & 0x0F00) &&
          (*(board + 0) & 0xF000) && (*(board + 1) & 0x000F) && (*(board + 1) & 0x00F0) &&
          (*(board + 1) & 0x0F00) && (*(board + 1) & 0xF000) && (*(board + 2) & 0x000F) &&
          (*(board + 2) & 0x00F0) && (*(board + 2) & 0x0F00) && (*(board + 2) & 0xF000) &&
          (*(board + 3) & 0x000F) && (*(board + 3) & 0x00F0) && (*(board + 3) & 0x0F00) &&
          (*(board + 3) & 0xF000)))
        goto addRandomTile;
    // 到這裡 board 就無空的 coil

    // (((*board + i) & 0x000F)>>x)     board 的x 項之間比對
    // ((*board + x) &  (0xF<<(i<<2)))  board[x]    之間比對
    for (uint8_t i = 0; i < 4; ++i) {
        if (!((((*(board + i) & 0x000F) >> 0) ^ ((*(board + i) & 0x00F0) >> 4)) &&
              (((*(board + i) & 0x00F0) >> 4) ^ ((*(board + i) & 0x0F00) >> 8)) &&
              (((*(board + i) & 0x0F00) >> 8) ^ ((*(board + i) & 0xF000) >> 12)))) {
            if (!(i ^ 3)) return 0b001; // 左右間是滿的 但ceil 間有相同的
        } else if (!(i ^ 0b11))
            break; // 左右間是滿的 且   ceil 間無相同的
    }
    for (uint8_t i = 0; i < 4; ++i) {
        if (!(((*(board + 0) & (0xF << (i << 2))) ^ (*(board + 1) & (0xF << (i << 2)))) &&
              ((*(board + 1) & (0xF << (i << 2))) ^ (*(board + 2) & (0xF << (i << 2)))) &&
              ((*(board + 2) & (0xF << (i << 2))) ^ (*(board + 3) & (0xF << (i << 2)))))) {
            if (i ^ 0b11) return 0x011; // 上下間是滿的 但ceil 間有相同的
            break;
        } else if (!(i ^ 0b11))
            return 0b111; // 死了
    }
addRandomTile:
    uint8_t pos;
    do {
        pos = random(0, 16);
    } while (*(board + (pos >> 2)) & (0xF << ((pos & 0b11) << 2)));
    *(board + (pos >> 2)) |= 0b0001 << ((pos & 0b11) << 2);
    return 0;
}
