#include "../include/Game2048.h"
#include "../include/Globals.h"
#include "../include/HardwareManager.h"
#include "Arduino.h"
#include <cstdint>

static GameStatusRecord initGame();

static void drawBoard(const uint16_t *const);
static uint8_t checkGameAliveAndAddRandomTile(uint16_t *),
    gameMoveAndCheckIsVictory(uint16_t *const map, const enum Direction direction, uint32_t &score);

static GameStatusRecord initGame() {
    GameStatusRecord gameDataRecord = {
        .map = {}, .score = 0, .lastMoveTime = 0, .dir = Direction::DIR_NONE};
    checkGameAliveAndAddRandomTile(gameDataRecord.map);
    return gameDataRecord;
}
// return  bit0
// 1 -> Lose / vectory 1/0
// 2 -> userInterrupt
uint8_t gameLoop() {
    GameStatusRecord gameDataRecord = initGame();
    // gameLoop -> while(cooldown < new_round_duration && back_to_menu_buttum unpressed)
    for (; (!isButtonPressed()) && ((millis() - gameDataRecord.lastMoveTime > MOVE_COOLDOWN));) {
        if (checkGameAliveAndAddRandomTile(gameDataRecord.map) & 0b100) return 1;
        // get Direction
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
        // end of get Direction

        if (!gameMoveAndCheckIsVictory(gameDataRecord.map, gameDataRecord.dir,
                                       gameDataRecord.score) ^2) {
            currentAppState = AppState::APP_CALIBRATE_2048;
            return 1;
        }

        drawBoard(gameDataRecord.map);
    }
    return 2;
}

// 我寫了三小
// return_data bit[n] -> 0 / 1
// 1 -> row is full / empty
// 2 -> col is full / empty
// 3 -> game is Dead/ Alive
static uint8_t checkGameAliveAndAddRandomTile(uint16_t *const map) {
    // 如果 map[row][col] 有一個 ceil 是空的則直接新增 tile
    if (!((*(map + 0) & 0x000F) && (*(map + 0) & 0x00F0) && (*(map + 0) & 0x0F00) &&
          (*(map + 0) & 0xF000) && (*(map + 1) & 0x000F) && (*(map + 1) & 0x00F0) &&
          (*(map + 1) & 0x0F00) && (*(map + 1) & 0xF000) && (*(map + 2) & 0x000F) &&
          (*(map + 2) & 0x00F0) && (*(map + 2) & 0x0F00) && (*(map + 2) & 0xF000) &&
          (*(map + 3) & 0x000F) && (*(map + 3) & 0x00F0) && (*(map + 3) & 0x0F00) &&
          (*(map + 3) & 0xF000)))
        goto addRandomTile;
    // 到這裡 map 就無空的 coil

    // (((*map + i) & 0x000F)>>x)     map 的x 項之間比對
    // ((*map + x) &  (0xF<<(i<<2)))  map[x]    之間比對
    for (uint8_t i = 0; i < 4; ++i) {
        if (!((((*(map + i) & 0x000F) >> 0) ^ ((*(map + i) & 0x00F0) >> 4)) &&
              (((*(map + i) & 0x00F0) >> 4) ^ ((*(map + i) & 0x0F00) >> 8)) &&
              (((*(map + i) & 0x0F00) >> 8) ^ ((*(map + i) & 0xF000) >> 12)))) {
            if (!(i ^ 3)) return 0b001; // 左右間是滿的 但ceil 間有相同的
        } else if (!(i ^ 0b11))
            break; // 左右間是滿的 且   ceil 間無相同的
    }
    for (uint8_t i = 0; i < 13; i += 4) { // 4 8 12
        if (!(((*(map + 0) & (0xF << i)) ^ (*(map + 1) & (0xF << i))) &&
              ((*(map + 1) & (0xF << i)) ^ (*(map + 2) & (0xF << i))) &&
              ((*(map + 2) & (0xF << i)) ^ (*(map + 3) & (0xF << i))))) {
            if (i ^ 0b11) return 0x011; // 上下間是滿的 但ceil 間有相同的
            break;
        } else if (!(i ^ 0b11))
            return 0b111; // 死了
    }
addRandomTile:
    uint8_t pos;
    do {
        pos = random(0, 16);
    } while (*(map + (pos >> 2)) & (0xF << ((pos & 0b11) << 2)));
    *(map + (pos >> 2)) |= 0b0001 << ((pos & 0b11) << 2);
    return 0;
}

// return ->
// 0 moved
// 1 Direction input is Direction::DIR_NONE
// 2 victory
static uint8_t gameMoveAndCheckIsVictory(uint16_t *const map, const enum Direction direction,
                                         uint32_t &score) {
    switch (direction) {
    case Direction::DIR_NONE: {
        return 1;
        break;
    }
    case Direction::DIR_UP: {
        for (uint8_t shift = 0; shift ^ 16; shift += 4) {
            for (uint8_t i = 0; i ^ 3; ++i) {
                uint16_t val1 = (*(map + i) >> shift) & 0xF;
                uint16_t val2 = (*(map + i + 1) >> shift) & 0xF;
                if (val1 && val1 == val2) {
                    *(map + i) = (*(map + i) & ~(0xF << shift)) | ((val1 + 1) << shift);
                    *(map + i + 1) = (*(map + i + 1) & ~(0xF << shift));
                    score += 2 << val1;
                }
            }
            for (uint8_t i = 0; i ^ 3; ++i) {
                uint16_t val = (*(map + i) >> shift) & 0xF;
                if (!val) {
                    for (uint8_t j = i; j ^ 3; ++j) {
                        uint16_t next = (*(map + j + 1) >> shift) & 0xF;
                        *(map + j) = (*(map + j) & ~(0xF << shift)) | (next << shift);
                    }
                    *(map + 3) &= ~(0xF << shift);
                    --i;
                }
            }
        }
        break;
    }
    case Direction::DIR_DOWN: {
        for (uint8_t shift = 0; shift ^ 16; shift += 4) {
            for (int8_t i = 2; i >= 0; --i) {
                uint16_t val1 = (*(map + i + 1) >> shift) & 0xF;
                uint16_t val2 = (*(map + i) >> shift) & 0xF;
                if (val1 && val1 == val2) {
                    *(map + i + 1) = (*(map + i + 1) & ~(0xF << shift)) | ((val1 + 1) << shift);
                    *(map + i) = (*(map + i) & ~(0xF << shift));
                    score += 2 << val1;
                }
            }
            for (int8_t i = 2; i >= 0; --i) {
                uint16_t val = (*(map + i) >> shift) & 0xF;
                if (!val) {
                    for (int8_t j = i; j > 0; --j) {
                        uint16_t prev = (*(map + j - 1) >> shift) & 0xF;
                        *(map + j) = (*(map + j) & ~(0xF << shift)) | (prev << shift);
                    }
                    *(map + 0) &= ~(0xF << shift);
                    ++i;
                }
            }
        }
        break;
    }
    case Direction::DIR_LEFT: {
        for (uint8_t row = 0; row ^ 4; ++row) {
            for (uint8_t i = 0; i ^ 3; ++i) {
                uint16_t val1 = (*(map + row) >> (i << 2)) & 0xF;
                uint16_t val2 = (*(map + row) >> ((i + 1) << 2)) & 0xF;
                if (val1 && val1 == val2) {
                    *(map + row) = (*(map + row) & ~(0xF << (i << 2))) | ((val1 + 1) << (i << 2));
                    *(map + row) &= ~(0xF << ((i + 1) << 2));
                    score += 2 << val1;
                }
            }
            for (uint8_t i = 0; i ^ 3; ++i) {
                uint16_t val = (*(map + row) >> (i << 2)) & 0xF;
                if (!val) {
                    for (uint8_t j = i; j ^ 3; ++j) {
                        uint16_t next = (*(map + row) >> ((j + 1) << 2)) & 0xF;
                        *(map + row) = (*(map + row) & ~(0xF << (j << 2))) | (next << (j << 2));
                    }
                    *(map + row) &= ~(0xF << (3 << 2));
                    --i;
                }
            }
        }
        break;
    }
    case Direction::DIR_RIGHT: {
        for (uint8_t row = 0; row ^ 4; ++row) {
            for (int8_t i = 2; i >= 0; --i) {
                uint16_t val1 = (*(map + row) >> ((i + 1) << 2)) & 0xF;
                uint16_t val2 = (*(map + row) >> (i << 2)) & 0xF;
                if (val1 && val1 == val2) {
                    *(map + row) =
                        (*(map + row) & ~(0xF << ((i + 1) << 2))) | ((val1 + 1) << ((i + 1) << 2));
                    *(map + row) &= ~(0xF << (i << 2));
                    score += 2 << val1;
                }
            }
            for (int8_t i = 2; i >= 0; --i) {
                uint16_t val = (*(map + row) >> (i << 2)) & 0xF;
                if (!val) {
                    for (int8_t j = i; j > 0; --j) {
                        uint16_t prev = (*(map + row) >> ((j - 1) << 2)) & 0xF;
                        *(map + row) = (*(map + row) & ~(0xF << (j << 2))) | (prev << (j << 2));
                    }
                    *(map + row) &= ~(0xF << 0);
                    ++i;
                }
            }
        }
        break;
    }
    }
    for (uint16_t i = 0xA; i; i <<= 4)
        if ((*(map + 0) & i) | (*(map + 1) & i) | (*(map + 2) & i) | (*(map + 3) & i)) return 2;
    return 0;
}

static void drawBoard(const uint16_t *const map) {
    display.fillScreen(ST77XX_BLACK);
    int SCREEN_W = display.width();
    int SCREEN_H = display.height();
    int gridSize = min(SCREEN_W, SCREEN_H);
    int cellW = gridSize >> 2;
    int cellH = cellW;
    int offsetX = (SCREEN_W - gridSize) >> 1;
    int offsetY = (SCREEN_H - gridSize) >> 1;

    for (int r = 0; r ^ 4; ++r) {
        for (int c = 0; c ^ 4; ++c) {
            int x = offsetX + c * cellW;
            int y = offsetY + r * cellH;
            display.drawRect(x, y, cellW, cellH, ST77XX_WHITE);

            uint8_t val = (map[r] >> (c << 2)) & 0xF;
            if (val) {
                display.setTextColor(ST77XX_WHITE);
                String num = String(1 << val);
                int16_t x1, y1;
                uint16_t w, h;
                display.getTextBounds(num, x, y, &x1, &y1, &w, &h);
                int textX = x + ((cellW - w) >> 1);
                int textY = y + ((cellH - h) >> 1);
                display.setCursor(textX, textY);
                display.print(num);
            }
        }
    }
}
