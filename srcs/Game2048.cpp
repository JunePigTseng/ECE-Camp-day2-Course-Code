#include "../include/Game2048.h"
#include "../include/HardwareManager.h"
#include <cstdint>

// Game configuration
const float RELATIVE_TILT_THRESHOLD = 4.0;
const uint32_t MOVE_COOLDOWN = 300;

// Game state variables
static uint16_t board[4][4];
static uint32_t score;
static bool victory;
static uint32_t lastMoveTime;
static bool isTiltedGate;
static Game2048State currentState;

// Internal Logic Methods
static void resetGame();
static void addRandomTile();
static void drawBoard();
static bool slideLine(uint16_t row[4]);
static bool moveLeft();
static bool moveRight();
static bool moveUp();
static bool moveDown();
static bool isGameOverLocal();

void initGame2048() {
    score = 0;
    victory = false;
    lastMoveTime = 0;
    isTiltedGate = false;
    currentState = GSTATE_PLAYING;
    resetGame();
}

void updateGame2048() {
    // If button pressed anywhere in game, exit to clock
    if (isButtonPressed()) {
        currentAppState = AppState::APP_CLOCK;
        return;
    }

    if (currentState == GSTATE_PLAYING) {
        if (millis() - lastMoveTime > MOVE_COOLDOWN) {
            Direction dir = Direction::DIR_NONE;

            sensors_event_t a, g, temp;
            mpu.getEvent(&a, &g, &temp);

            // Map to relative tilts
            float dX = a.acceleration.x - getBaseX();
            float dY = a.acceleration.y - getBaseY();

            if (dX > RELATIVE_TILT_THRESHOLD)
                dir = Direction::DIR_LEFT;
            else if (dX < -RELATIVE_TILT_THRESHOLD)
                dir = Direction::DIR_RIGHT;
            else if (dY > RELATIVE_TILT_THRESHOLD)
                dir = Direction::DIR_DOWN;
            else if (dY < -RELATIVE_TILT_THRESHOLD)
                dir = Direction::DIR_UP;
            // 轉換型態成uint8_t 後去判斷是否相同 只用 1*CLK
            if (static_cast<uint8_t>(dir) ^ static_cast<uint8_t>(Direction::DIR_NONE)) {
                if (!isTiltedGate) {
                    bool moved = false;
                    switch (dir) {
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
                    default:break;
                    }

                    if (moved) {
                        addRandomTile();
                        if (isGameOverLocal()) {
                            currentState = GSTATE_GAMEOVER;
                        }
                    }
                    isTiltedGate = true;
                    lastMoveTime = millis();
                }
            } else {
                isTiltedGate = false; // Sensor is relatively flat again
            }
        }
        drawBoard();
    } else if (currentState == GSTATE_GAMEOVER) {
        display.fillScreen(ST77XX_BLACK);
        display.setTextSize(1);
        display.setTextColor(ST77XX_WHITE);
        display.setCursor(10, 30);
        if (victory)
            display.print(F("YOU WON!"));
        else
            display.print(F("GAME OVER"));

        display.setCursor(10, 60);
        display.print(F("Score: "));
        display.print(score);

        display.setCursor(10, 90);
        display.print(F("Press Btn to Exit"));
    }
}

static void resetGame() {
    memset(board, 0, sizeof(board));
    score = 0;
    victory = false;
    addRandomTile();
    addRandomTile();
}

static void drawBoard() {
    display.fillScreen(ST77XX_BLACK);
    int SCREEN_W = display.width();
    int SCREEN_H = display.height();
    // Use a square area for the 4x4 grid
    int gridSize = min(SCREEN_W, SCREEN_H);
    int cellW = gridSize / 4;
    int cellH = cellW; // Keep cells square
    int offsetX = (SCREEN_W - gridSize) / 2;
    int offsetY = (SCREEN_H - gridSize) / 2;

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int x = offsetX + c * cellW;
            int y = offsetY + r * cellH;

            display.drawRect(x, y, cellW, cellH, ST77XX_WHITE);

            if (board[r][c] > 0) {
                display.setTextColor(ST77XX_WHITE);
                String num = String(board[r][c]);
                int16_t x1, y1;
                uint16_t w, h;
                display.getTextBounds(num, x, y, &x1, &y1, &w, &h);

                int textX = x + (cellW - w) / 2;
                int textY = y + (cellH - h) / 2;
                display.setCursor(textX, textY);
                display.print(num);
            }
        }
    }
}

static void addRandomTile() {
    int emptySpaces[16][2];
    int count = 0;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (board[r][c] == 0) {
                emptySpaces[count][0] = r;
                emptySpaces[count][1] = c;
                count++;
            }
        }
    }
    if (count > 0) {
        int idx = random(0, count);
        int r = emptySpaces[idx][0];
        int c = emptySpaces[idx][1];
        board[r][c] = (random(0, 10) < 9) ? 2 : 4;
    }
}

static bool slideLine(uint16_t row[4]) {
    bool moved = false;
    for (int i = 0; i < 3; i++) {
        if (row[i] == 0) {
            for (int j = i + 1; j < 4; j++) {
                if (row[j] != 0) {
                    row[i] = row[j];
                    row[j] = 0;
                    moved = true;
                    break;
                }
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        if (row[i] != 0 && row[i] == row[i + 1]) {
            row[i] *= 2;
            score += row[i];
            if (row[i] == 2048)
                victory = true;
            row[i + 1] = 0;
            moved = true;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (row[i] == 0) {
            for (int j = i + 1; j < 4; j++) {
                if (row[j] != 0) {
                    row[i] = row[j];
                    row[j] = 0;
                    moved = true;
                    break;
                }
            }
        }
    }
    return moved;
}

static bool moveLeft() {
    bool moved = false;
    for (int r = 0; r < 4; r++) {
        if (slideLine(board[r]))
            moved = true;
    }
    return moved;
}

static bool moveRight() {
    bool moved = false;
    for (int r = 0; r < 4; r++) {
        uint16_t temp[4] = {board[r][3], board[r][2], board[r][1], board[r][0]};
        if (slideLine(temp)) {
            board[r][0] = temp[3];
            board[r][1] = temp[2];
            board[r][2] = temp[1];
            board[r][3] = temp[0];
            moved = true;
        }
    }
    return moved;
}

static bool moveUp() {
    bool moved = false;
    for (int c = 0; c < 4; c++) {
        uint16_t temp[4] = {board[0][c], board[1][c], board[2][c], board[3][c]};
        if (slideLine(temp)) {
            board[0][c] = temp[0];
            board[1][c] = temp[1];
            board[2][c] = temp[2];
            board[3][c] = temp[3];
            moved = true;
        }
    }
    return moved;
}

static bool moveDown() {
    bool moved = false;
    for (int c = 0; c < 4; c++) {
        uint16_t temp[4] = {board[3][c], board[2][c], board[1][c], board[0][c]};
        if (slideLine(temp)) {
            board[3][c] = temp[0];
            board[2][c] = temp[1];
            board[1][c] = temp[2];
            board[0][c] = temp[3];
            moved = true;
        }
    }
    return moved;
}

static bool isGameOverLocal() {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (board[r][c] == 0)
                return false;
        }
    }
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (c < 3 && board[r][c] == board[r][c + 1])
                return false;
            if (r < 3 && board[r][c] == board[r + 1][c])
                return false;
        }
    }
    return true;
}
