#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

// --- Configuration ---
// OLED Display Dimension (Change SCREEN_HEIGHT to 128 for your future screen)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Accelerometer Module
Adafruit_MPU6050 mpu;

// Pin for restart / start button (connect this pin to a button, and the other side to GND)
#define BUTTON_PIN 2

// Game constants
#define TILT_THRESHOLD 5.0  // Sensitivity, approx >5 means ~30 degrees tilt
#define MOVE_COOLDOWN 200   // Milliseconds minimum between inputs

// --- Game State ---
uint16_t board[4][4];
uint32_t score = 0;
bool victory = false;
uint32_t lastMoveTime = 0;
bool isTilted = false; // Ensures one move per physical tilt

enum GameState {
  STATE_START,
  STATE_PLAYING,
  STATE_GAMEOVER
};
GameState currentState = STATE_START;

enum Direction {
  DIR_NONE,
  DIR_UP,
  DIR_DOWN,
  DIR_LEFT,
  DIR_RIGHT
};

// --- Logic Prototypes ---
void addRandomTile();
bool moveLeft();
bool moveRight();
bool moveUp();
bool moveDown();
bool isGameOver();

// --- Hardware Init ---
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize Display (Address 0x3C is extremely common)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Init 2048...");
  display.println("Connecting Sensor...");
  display.display();
  
  // Initialize MPU6050
  if (!mpu.begin()) {
    display.println("MPU6050 failed!");
    display.display();
    while (1) { delay(10); } // Stop on failure
  }
  
  // Range scaling helps noise reduction
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  // Seed random from disconnected analog pin
  randomSeed(analogRead(A0));
  
  // Setup waiting screen
  display.clearDisplay();
  display.setCursor(10, 10);
  display.setTextSize(2);
  display.print("2048");
  display.setTextSize(1);
  display.setCursor(10, 40);
  display.print("Press Btn to Start");
  display.display();
}

// --- Main State Machine ---
void loop() {
  // Button Debouncing logic
  static bool lastBtnState = HIGH;
  bool btnState = digitalRead(BUTTON_PIN);
  
  if (btnState == LOW && lastBtnState == HIGH) {
    // Starting or Restarting the Game
    memset(board, 0, sizeof(board));
    score = 0;
    victory = false;
    addRandomTile();
    addRandomTile();
    currentState = STATE_PLAYING;
    delay(200); // Wait out the button bounce
  }
  lastBtnState = btnState;

  if (currentState == STATE_PLAYING) {
    if (millis() - lastMoveTime > MOVE_COOLDOWN) {
      
      Direction dir = DIR_NONE;
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      
      // Determine device tilt
      // (Mapping may need adjustment based on sensor physical orientation)
      if (a.acceleration.x > TILT_THRESHOLD) dir = DIR_LEFT;
      else if (a.acceleration.x < -TILT_THRESHOLD) dir = DIR_RIGHT;
      else if (a.acceleration.y > TILT_THRESHOLD) dir = DIR_DOWN;
      else if (a.acceleration.y < -TILT_THRESHOLD) dir = DIR_UP;

      // Ensure that a user must return to flat before performing another move
      if (dir != DIR_NONE) {
        if (!isTilted) {
          bool moved = false;
          if (dir == DIR_LEFT) moved = moveLeft();
          else if (dir == DIR_RIGHT) moved = moveRight();
          else if (dir == DIR_UP) moved = moveUp();
          else if (dir == DIR_DOWN) moved = moveDown();
          
          if (moved) {
            addRandomTile();
            if (isGameOver()) {
              currentState = STATE_GAMEOVER;
            }
          }
          isTilted = true;
          lastMoveTime = millis();
        }
      } else {
        isTilted = false; // Sensor is flat again
      }
    }
    drawBoard();
  }
  else if (currentState == STATE_GAMEOVER) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 10);
    if (victory) display.print("YOU WON!");
    else display.print("GAME OVER");
    
    display.setCursor(10, 30);
    display.print("Score: ");
    display.print(score);
    
    display.setCursor(10, 50);
    display.print("Press Btn to Reset");
    display.display();
  }
}

// --- Graphical Rendering ---
void drawBoard() {
  display.clearDisplay();
  int cellW = SCREEN_WIDTH / 4;
  int cellH = SCREEN_HEIGHT / 4;
  
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      int x = c * cellW;
      int y = r * cellH;
      
      // Draw Grid Line Rectangle
      display.drawRect(x, y, cellW, cellH, SSD1306_WHITE);
      
      if (board[r][c] > 0) {
        String num = String(board[r][c]);
        int16_t x1, y1;
        uint16_t w, h;
        // Allows centering dynamic text perfectly in the box
        display.getTextBounds(num, x, y, &x1, &y1, &w, &h);
        
        int textX = x + (cellW - w) / 2;
        int textY = y + (cellH - h) / 2;
        display.setCursor(textX, textY);
        display.print(num);
      }
    }
  }
  display.display();
}

// --- 2048 Game Engine ---
void addRandomTile() {
  int emptySpaces[16][2];
  int count = 0;
  
  for (int r=0; r<4; r++) {
    for (int c=0; c<4; c++) {
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
    board[r][c] = (random(0, 10) < 9) ? 2 : 4; // 90% chance for 2
  }
}

// Slide helper matching array pointers
bool slideLine(uint16_t row[4]) {
  bool moved = false;
  // 1. Shift non-zeros forward
  for (int i=0; i<3; i++) {
    if (row[i] == 0) {
      for (int j=i+1; j<4; j++) {
        if (row[j] != 0) {
          row[i] = row[j];
          row[j] = 0;
          moved = true;
          break;
        }
      }
    }
  }
  // 2. Perform Merges
  for (int i=0; i<3; i++) {
    if (row[i] != 0 && row[i] == row[i+1]) {
      row[i] *= 2;
      score += row[i];
      if (row[i] == 2048) victory = true;
      row[i+1] = 0;
      moved = true;
    }
  }
  // 3. Shift forward again post-merge
  for (int i=0; i<3; i++) {
    if (row[i] == 0) {
      for (int j=i+1; j<4; j++) {
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

bool moveLeft() {
  bool moved = false;
  for (int r=0; r<4; r++) {
    if (slideLine(board[r])) moved = true;
  }
  return moved;
}

bool moveRight() {
  bool moved = false;
  for (int r=0; r<4; r++) {
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

bool moveUp() {
  bool moved = false;
  for (int c=0; c<4; c++) {
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

bool moveDown() {
  bool moved = false;
  for (int c=0; c<4; c++) {
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

bool isGameOver() {
  // Check empty cells
  for (int r=0; r<4; r++) {
    for (int c=0; c<4; c++) {
      if (board[r][c] == 0) return false;
    }
  }
  // Check adjacent matching cells
  for (int r=0; r<4; r++) {
    for (int c=0; c<4; c++) {
      if (c < 3 && board[r][c] == board[r][c+1]) return false;
      if (r < 3 && board[r][c] == board[r+1][c]) return false;
    }
  }
  return true; // No moves left
}
