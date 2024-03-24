// UVA HACKATHON 2024
// Quest for Meowtroplis
// Made by Aislinn Jones and Nolan Hill
// ------------------------------------

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <SPI.h>
#include "pitches.h"
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4

#define BUTTON A6

#define ACCELX A8 // accelerometer xyz
#define ACCELY A9
#define ACCELZ A10

// colors to maybe use:
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xf483
#define GRAY 0xa4d1
#define PINK 0xe558
#define BROWN 0x8b06
#define LBROWN 0xc52f
#define LGREEN 0x8d8f
#define TRINK 0xed9a

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void)
{
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9325)
  {
    Serial.println(F("Found ILI9325 LCD driver"));
  }
  else if (identifier == 0x9328)
  {
    Serial.println(F("Found ILI9328 LCD driver"));
  }
  else if (identifier == 0x4535)
  {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }
  else if (identifier == 0x7575)
  {
    Serial.println(F("Found HX8347G LCD driver"));
  }
  else if (identifier == 0x9341)
  {
    Serial.println(F("Found ILI9341 LCD driver"));
  }
  else if (identifier == 0x8357)
  {
    Serial.println(F("Found HX8357D LCD driver"));
  }
  else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  }
  else
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0x9341;
  }

  tft.begin(identifier);

  Serial.println(F("Benchmark                Time (microseconds)"));
  tft.setRotation(2);
  Serial.print(F("Text"));
  Serial.println(titleScreen());
  delay(5000);
  tft.fillRect(0, 0, 240, 245, BLUE);
  tft.fillRect(0, 245, 240, 75, GREEN);

  Serial.println(F("Done!"));
}

// Game variables
int cloudY = rand() % 60 + 60;
int cloudX = 240;
int cloud2Y = 0;
int score = 0;
bool isCloudPresent = false;
int playerY = 210;
int playerYDynamic = 210;
int momentum = 0;
bool isPressed = false;
bool goingDown = false;
int obstacleX = 240;
int obstacleW = 20;
int obstacleH = 20;
bool isObstaclePresent = false;
bool isGameOver = false;
int gameOverSequence = 5;
float ACCELEROMETER_THRESHOLD = 1.75;
int quakeSequence = -1;
int quakePlayerY = 0;
int isYarnPresent = false;
bool yarnDeflect = false;
int yarnX = 300;
int yarnY = 0;
bool isJumpPlaying = false;
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int xMin = 263;
int xMax = 393;
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int yMin = 264;
int yMax = 395;
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int zMin = 267;
int zMax = 397;
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMM

void loop(void)
{
  if (isGameOver)
  {
    gameOver(score);
    if (gameOverSequence >= 0)
      gameOverSequence -= 1;
    delay(100);
    if (gameOverSequence == -1 && digitalRead(BUTTON) == HIGH)
    {
      isGameOver = false;
      isObstaclePresent = false;
      obstacleX = 240;
      score = 0;
      yarnX = 300;
      yarnY = 0;
      isYarnPresent = false;
      tft.fillRect(0, 0, 240, 245, BLUE);
      gameOverSequence = 5;
    }
  }
  else
  {

    collisionCheck();

    if (digitalRead(BUTTON) == HIGH)
    {
      jump();
    }
    if (digitalRead(BUTTON) == LOW)
    {
      if (momentum < 0)
      {
        goingDown = true;
        momentum += 20;
      }
      else
        goingDown = false;
      playerY = 210 + momentum;
      isPressed = false;
    }

    if (!isCloudPresent && rand() % 15 == 1)
    {
      drawCloud();
    }
    if (isCloudPresent)
    {
      cloudX -= 35;
      drawCloud();
    }

    displayScore();
    for (uint8_t frame = 0; frame < 2; frame++)
    {
      if (playerY != 210)
        frame = 2;
      drawCharacter(frame);
    }

    if (!isObstaclePresent && rand() % 10 == 1)
    {
      sendObstacle();
    }
    if (isObstaclePresent)
    {
      obstacleX -= 30;
      sendObstacle();
    }

    if (!isYarnPresent && rand() % 20 == 1)
    {
      sendYarn();
    }
    if (isYarnPresent)
    {
      if (!yarnDeflect)
      {
        yarnX -= 15;
        yarnY += 15;
      }
      else
        yarnY -= 15;
      sendYarn();
    }

    checkAccel();
  }
}

unsigned long titleScreen()
{
  unsigned long start = micros();
  tft.fillScreen(CYAN);
  tft.setCursor(60, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.println("QUEST");
  tft.setCursor(85, 50);
  tft.println("FOR");
  tft.setTextSize(3.75);
  tft.setCursor(10, 90);
  tft.println("MEOWTROPOLIS");

  // drawing big cat for the main title screen
  tft.fillRect(60, 120, 30, 10, ORANGE);
  tft.fillRect(160, 130, 30, 10, ORANGE);
  tft.fillRect(60, 130, 10, 10, ORANGE);
  tft.fillRect(70, 130, 10, 10, BROWN);
  tft.fillRect(80, 130, 10, 10, ORANGE);
  tft.fillRect(160, 130, 10, 10, ORANGE);
  tft.fillRect(170, 130, 10, 10, BROWN);
  tft.fillRect(170, 140, 20, 10, BROWN);
  tft.fillRect(180, 130, 20, 10, ORANGE);
  tft.fillRect(50, 140, 20, 10, ORANGE);
  tft.fillRect(70, 140, 20, 10, BROWN);
  tft.fillRect(90, 140, 10, 10, ORANGE);
  tft.fillRect(150, 140, 20, 10, ORANGE);
  tft.fillRect(190, 140, 10, 10, ORANGE);
  tft.fillRect(50, 150, 10, 10, ORANGE);
  tft.fillRect(60, 150, 30, 10, BROWN);
  tft.fillRect(90, 150, 10, 10, ORANGE);
  tft.fillRect(150, 150, 10, 10, ORANGE);
  tft.fillRect(160, 150, 30, 10, BROWN);
  tft.fillRect(190, 150, 20, 10, ORANGE);
  tft.fillRect(30, 160, 30, 10, GRAY);
  tft.fillRect(60, 160, 120, 20, ORANGE);
  tft.fillRect(200, 160, 10, 10, ORANGE);
  tft.fillRect(210, 160, 20, 10, GRAY);
  tft.fillRect(200, 170, 20, 10, GRAY);
  tft.fillRect(30, 160, 30, 10, GRAY);
  tft.fillRect(50, 170, 10, 10, GRAY);
  tft.fillRect(20, 180, 30, 10, GRAY);
  tft.fillRect(50, 180, 30, 10, ORANGE);
  tft.fillRect(50, 190, 150, 50, ORANGE);
  tft.fillRect(80, 180, 20, 20, WHITE);
  tft.fillRect(50, 190, 10, 10, GRAY);
  tft.fillRect(90, 190, 10, 10, BLACK);
  tft.fillRect(80, 220, 10, 10, BLACK);
  tft.fillRect(150, 180, 20, 20, WHITE);
  tft.fillRect(150, 190, 10, 10, BLACK);
  tft.fillRect(200, 190, 30, 10, GRAY);
  tft.fillRect(30, 200, 20, 10, GRAY);
  tft.fillRect(120, 200, 10, 10, LBROWN);
  tft.fillRect(130, 200, 10, 10, PINK);
  tft.fillRect(120, 210, 10, 10, PINK);
  tft.fillRect(80, 220, 10, 10, BLACK);
  tft.fillRect(90, 230, 20, 10, BLACK);
  tft.fillRect(120, 220, 10, 10, BLACK);
  tft.fillRect(150, 220, 10, 10, BLACK);
  tft.fillRect(60, 230, 120, 10, ORANGE);
  tft.fillRect(50, 230, 10, 10, GRAY);
  tft.fillRect(90, 230, 20, 10, BLACK);
  tft.fillRect(130, 230, 20, 10, BLACK);
  tft.fillRect(150, 230, 10, 10, BLACK);
  tft.fillRect(200, 190, 30, 10, GRAY);
  tft.fillRect(200, 200, 10, 20, ORANGE);
  tft.fillRect(210, 210, 20, 10, GRAY);
  tft.fillRect(200, 220, 10, 10, GRAY);
  tft.fillRect(60, 240, 20, 10, GRAY);
  tft.fillRect(80, 240, 90, 10, ORANGE);
  tft.fillRect(180, 240, 10, 10, GRAY);
  tft.fillRect(80, 250, 90, 10, GRAY);
  tft.fillRect(160, 120, 30, 10, ORANGE);
  tft.fillRect(200, 180, 10, 10, ORANGE);
  tft.fillRect(180, 160, 10, 30, ORANGE);
  tft.fillRect(170, 180, 20, 10, ORANGE);
  tft.fillRect(100, 180, 50, 10, ORANGE);
  tft.fillRect(170, 240, 10, 10, GRAY);
  tft.fillRect(190, 160, 10, 30, ORANGE);
  tft.fillRect(110, 230, 10, 10, BLACK);
}

unsigned long drawCharacter(uint8_t frame)
{
  switch (frame)
  {
  case 0:
    tft.fillRect(62, 235, 10, 8, BLUE);
    tft.fillRect(68, 222, 6, 12, BLUE);
    tft.fillRect(58, 219, 12, 4, ORANGE);
    tft.fillRect(56, 218, 2, 6, ORANGE);
    tft.fillRect(67, 215, 3, 3, ORANGE);
    tft.fillRect(68, 216, 3, 5, ORANGE);
    tft.fillRect(57, 210, 1, 1, ORANGE);
    tft.fillRect(56, 211, 3, 1, ORANGE);
    tft.fillRect(55, 212, 5, 1, ORANGE);
    tft.fillRect(55, 213, 2, 1, ORANGE);
    tft.fillRect(57, 213, 1, 1, BROWN);
    tft.fillRect(58, 213, 2, 1, ORANGE);
    tft.fillRect(55, 214, 1, 1, ORANGE);
    tft.fillRect(56, 214, 3, 1, BROWN);
    tft.fillRect(59, 214, 1, 1, ORANGE);
    tft.fillRect(55, 215, 14, 4, ORANGE);
    tft.fillRect(70, 216, 1, 1, ORANGE);
    tft.fillRect(67, 211, 2, 1, ORANGE);
    tft.fillRect(66, 212, 2, 1, ORANGE);
    tft.fillRect(65, 213, 5, 2, ORANGE);
    tft.fillRect(67, 215, 3, 2, BROWN);
    tft.fillRect(68, 212, 1, 1, BROWN);
    tft.fillRect(69, 211, 1, 2, ORANGE);
    tft.fillRect(55, 215, 14, 5, ORANGE);
    tft.fillRect(70, 216, 1, 13, ORANGE);
    tft.fillRect(71, 217, 1, 5, ORANGE);
    tft.fillRect(72, 217, 1, 1, GRAY);
    tft.fillRect(72, 219, 2, 1, GRAY);
    tft.fillRect(60, 216, 2, 2, WHITE);
    tft.fillRect(61, 217, 1, 1, BLACK);
    tft.fillRect(65, 216, 2, 2, WHITE);
    tft.fillRect(66, 217, 1, 1, BLACK);
    tft.fillRect(62, 219, 1, 1, PINK);
    tft.fillRect(61, 220, 1, 1, BLACK);
    tft.fillRect(62, 221, 1, 1, BLACK);
    tft.fillRect(63, 220, 1, 1, BLACK);
    tft.fillRect(64, 221, 2, 1, BLACK);
    tft.fillRect(66, 220, 1, 1, BLACK);
    tft.fillRect(54, 217, 3, 1, GRAY);
    tft.fillRect(54, 219, 2, 1, GRAY);
    tft.fillRect(56, 221, 1, 1, GRAY);
    tft.fillRect(57, 222, 1, 1, GRAY);
    tft.fillRect(58, 223, 12, 1, GRAY);
    tft.fillRect(70, 222, 1, 1, GRAY);
    tft.fillRect(71, 221, 1, 1, GRAY);
    tft.fillRect(70, 223, 2, 1, ORANGE);
    tft.fillRect(54, 223, 4, 1, ORANGE);
    tft.fillRect(52, 232, 2, 2, ORANGE);
    tft.fillRect(54, 233, 1, 2, ORANGE);
    tft.fillRect(55, 233, 2, 2, ORANGE);
    tft.fillRect(54, 222, 3, 7, ORANGE);
    tft.fillRect(53, 229, 2, 2, ORANGE);
    tft.fillRect(54, 229, 1, 1, GRAY);
    tft.fillRect(53, 230, 1, 1, GRAY);
    tft.fillRect(57, 224, 13, 11, ORANGE);
    tft.fillRect(57, 235, 5, 6, ORANGE);
    tft.fillRect(57, 241, 4, 1, ORANGE);
    tft.fillRect(58, 242, 3, 1, ORANGE);
    tft.fillRect(57, 243, 3, 2, ORANGE);
    tft.fillRect(58, 243, 1, 1, GRAY);
    tft.fillRect(57, 244, 1, 1, GRAY);
    tft.fillRect(59, 244, 1, 1, GRAY);
    tft.fillRect(63, 235, 5, 6, ORANGE);
    tft.fillRect(64, 241, 4, 2, ORANGE);
    tft.fillRect(65, 243, 4, 2, ORANGE);
    tft.fillRect(67, 243, 1, 1, GRAY);
    tft.fillRect(66, 244, 1, 1, GRAY);
    tft.fillRect(68, 244, 1, 1, GRAY);
    tft.fillRect(70, 224, 2, 5, ORANGE);
    tft.fillRect(72, 226, 1, 1, ORANGE);
    tft.fillRect(72, 227, 2, 3, ORANGE);
    tft.fillRect(73, 228, 2, 3, ORANGE);
    tft.fillRect(73, 229, 1, 1, GRAY);
    tft.fillRect(74, 230, 1, 1, GRAY);
    // tail shorter cover up tail too
    tft.fillRect(50, playerY + 14, 2, 8, BLUE);
    tft.fillRect(50, playerY, 2, 14, BLUE);
    tft.fillRect(60, playerY, 5, 5, BLUE);
    tft.fillRect(60, playerY+2,5,3, BLUE);
    tft.fillRect(59, playerY+1, 8,1,BLUE);
    tft.fillRect(65,playerY+2, 1, 1, BLUE);
    tft.fillRect(58,playerY,13,1,BLUE);

    break;
  case 1:
    tft.fillRect(58, 219, 12, 4, ORANGE);
    tft.fillRect(56, 218, 2, 6, ORANGE);
    tft.fillRect(67, 215, 3, 3, ORANGE);
    tft.fillRect(68, 216, 3, 5, ORANGE);
    tft.fillRect(57, 210, 1, 1, ORANGE);
    tft.fillRect(56, 211, 3, 1, ORANGE);
    tft.fillRect(55, 212, 5, 1, ORANGE);
    tft.fillRect(55, 213, 2, 1, ORANGE);
    tft.fillRect(57, 213, 1, 1, BROWN);
    tft.fillRect(58, 213, 2, 1, ORANGE);
    tft.fillRect(55, 214, 1, 1, ORANGE);
    tft.fillRect(56, 214, 3, 1, BROWN);
    tft.fillRect(59, 214, 1, 1, ORANGE);
    tft.fillRect(55, 215, 14, 4, ORANGE);
    tft.fillRect(70, 216, 1, 1, ORANGE);
    tft.fillRect(67, 211, 2, 1, ORANGE);
    tft.fillRect(66, 212, 2, 1, ORANGE);
    tft.fillRect(65, 213, 5, 2, ORANGE);
    tft.fillRect(67, 215, 3, 2, BROWN);
    tft.fillRect(68, 212, 1, 1, BROWN);
    tft.fillRect(69, 211, 1, 2, ORANGE);
    tft.fillRect(55, 215, 14, 5, ORANGE);
    tft.fillRect(70, 216, 1, 13, ORANGE);
    tft.fillRect(71, 217, 1, 5, ORANGE);
    tft.fillRect(72, 217, 1, 1, GRAY);
    tft.fillRect(72, 219, 2, 1, GRAY);
    tft.fillRect(60, 216, 2, 2, WHITE);
    tft.fillRect(61, 217, 1, 1, BLACK);
    tft.fillRect(65, 216, 2, 2, WHITE);
    tft.fillRect(66, 217, 1, 1, BLACK);
    tft.fillRect(62, 219, 1, 1, PINK);
    tft.fillRect(61, 220, 1, 1, BLACK);
    tft.fillRect(62, 221, 1, 1, BLACK);
    tft.fillRect(63, 220, 1, 1, BLACK);
    tft.fillRect(64, 221, 2, 1, BLACK);
    tft.fillRect(66, 220, 1, 1, BLACK);
    tft.fillRect(54, 217, 3, 1, GRAY);
    tft.fillRect(54, 219, 2, 1, GRAY);
    tft.fillRect(56, 221, 1, 1, GRAY);
    tft.fillRect(57, 222, 1, 1, GRAY);
    tft.fillRect(58, 223, 12, 1, GRAY);
    tft.fillRect(70, 222, 1, 1, GRAY);
    tft.fillRect(71, 221, 1, 1, GRAY);
    tft.fillRect(70, 223, 2, 1, ORANGE);
    tft.fillRect(54, 223, 4, 1, ORANGE);
    tft.fillRect(52, 232, 2, 2, ORANGE);
    tft.fillRect(54, 233, 1, 2, ORANGE);
    tft.fillRect(55, 233, 2, 2, ORANGE);
    tft.fillRect(54, 222, 3, 7, ORANGE);
    tft.fillRect(53, 229, 2, 2, ORANGE);
    tft.fillRect(54, 229, 1, 1, GRAY);
    tft.fillRect(53, 230, 1, 1, GRAY);
    tft.fillRect(57, 224, 13, 11, ORANGE);
    tft.fillRect(57, 235, 5, 6, ORANGE);
    tft.fillRect(57, 241, 4, 1, ORANGE);
    tft.fillRect(58, 242, 3, 1, ORANGE);
    tft.fillRect(57, 243, 3, 2, ORANGE);
    tft.fillRect(58, 243, 1, 1, GRAY);
    tft.fillRect(57, 244, 1, 1, GRAY);
    tft.fillRect(59, 244, 1, 1, GRAY);
    tft.fillRect(63, 235, 5, 6, ORANGE);
    tft.fillRect(64, 241, 4, 2, ORANGE);
    tft.fillRect(65, 243, 4, 2, ORANGE);
    tft.fillRect(67, 243, 1, 1, GRAY);
    tft.fillRect(66, 244, 1, 1, GRAY);
    tft.fillRect(68, 244, 1, 1, GRAY);
    tft.fillRect(70, 224, 2, 5, ORANGE);
    tft.fillRect(72, 226, 1, 1, ORANGE);
    tft.fillRect(72, 227, 2, 3, ORANGE);
    tft.fillRect(73, 228, 2, 3, ORANGE);
    tft.fillRect(73, 229, 1, 1, GRAY);
    tft.fillRect(74, 230, 1, 1, GRAY);
    tft.fillRect(70, 223, 5, 10, BLUE);
    tft.fillRect(70, 224, 1, 4, ORANGE);
    tft.fillRect(71, 223, 1, 4, ORANGE);
    tft.fillRect(72, 223, 1, 3, ORANGE);
    tft.fillRect(73, 222, 1, 3, ORANGE);
    tft.fillRect(74, 221, 1, 3, ORANGE);
    tft.fillRect(62, 235, 9, 10, BLUE);
    tft.fillRect(65, 235, 6, 4, ORANGE);
    tft.fillRect(66, 239, 6, 1, ORANGE);
    tft.fillRect(67, 240, 5, 1, ORANGE);
    tft.fillRect(68, 241, 5, 1, ORANGE);
    tft.fillRect(69, 242, 4, 1, ORANGE);
    tft.fillRect(50, 224, 2, 8, BLUE);
    tft.fillRect(50, 234, 2, 11, ORANGE);

    break;
  case 2:
    // player y is 210
    tft.fillRect(50, playerY + 34, 2, 1, BLUE);
    tft.fillRect(58, playerY + 9, 12, 4, ORANGE);
    tft.fillRect(56, playerY + 8, 2, 6, ORANGE);
    tft.fillRect(67, playerY + 5, 3, 3, ORANGE);
    tft.fillRect(68, playerY + 6, 3, 5, ORANGE);
    tft.fillRect(57, playerY, 1, 1, ORANGE);
    tft.fillRect(56, playerY + 1, 3, 1, ORANGE);
    tft.fillRect(55, playerY + 2, 5, 1, ORANGE);
    tft.fillRect(55, playerY + 3, 2, 1, ORANGE);
    tft.fillRect(57, playerY + 3, 1, 1, BROWN);
    tft.fillRect(58, playerY + 3, 2, 1, ORANGE);
    tft.fillRect(55, playerY + 4, 1, 1, ORANGE);
    tft.fillRect(56, playerY + 4, 3, 1, BROWN);
    tft.fillRect(59, playerY + 4, 1, 1, ORANGE);
    tft.fillRect(55, playerY + 5, 14, 4, ORANGE);
    tft.fillRect(70, playerY + 6, 1, 1, ORANGE);
    tft.fillRect(67, playerY + 1, 2, 1, ORANGE);
    tft.fillRect(66, playerY + 2, 2, 1, ORANGE);
    tft.fillRect(65, playerY + 3, 5, 2, ORANGE);
    tft.fillRect(67, playerY + 5, 3, 2, BROWN);
    tft.fillRect(68, playerY + 2, 1, 1, BROWN);
    tft.fillRect(69, playerY + 1, 1, 2, ORANGE);
    tft.fillRect(55, playerY + 5, 14, 5, ORANGE);
    tft.fillRect(70, playerY + 6, 1, 13, ORANGE);
    tft.fillRect(71, playerY + 7, 1, 5, ORANGE);
    tft.fillRect(72, playerY + 7, 1, 1, GRAY);
    tft.fillRect(72, playerY + 9, 2, 1, GRAY);
    tft.fillRect(60, playerY + 6, 2, 2, WHITE);
    tft.fillRect(61, playerY + 7, 1, 1, BLACK);
    tft.fillRect(65, playerY + 6, 2, 2, WHITE);
    tft.fillRect(66, playerY + 7, 1, 1, BLACK);
    tft.fillRect(62, playerY + 9, 1, 1, PINK);
    tft.fillRect(61, playerY + 10, 1, 1, BLACK);
    tft.fillRect(62, playerY + 11, 1, 1, BLACK);
    tft.fillRect(63, playerY + 10, 1, 1, BLACK);
    tft.fillRect(64, playerY + 11, 2, 1, BLACK);
    tft.fillRect(66, playerY + 10, 1, 1, BLACK);
    tft.fillRect(54, playerY + 7, 3, 1, GRAY);
    tft.fillRect(54, playerY + 9, 2, 1, GRAY);
    tft.fillRect(56, playerY + 11, 1, 1, GRAY);
    tft.fillRect(57, playerY + 12, 1, 1, GRAY);
    tft.fillRect(58, playerY + 13, 12, 1, GRAY);
    tft.fillRect(70, playerY + 12, 1, 1, GRAY);
    tft.fillRect(71, playerY + 11, 1, 1, GRAY);
    tft.fillRect(70, playerY + 13, 2, 1, ORANGE);
    tft.fillRect(54, playerY + 13, 4, 1, ORANGE);
    tft.fillRect(52, playerY + 22, 2, 2, ORANGE);
    tft.fillRect(54, playerY + 23, 1, 2, ORANGE);
    tft.fillRect(55, playerY + 23, 2, 2, ORANGE);
    tft.fillRect(54, playerY + 12, 3, 7, ORANGE);
    tft.fillRect(53, playerY + 19, 2, 2, ORANGE);
    tft.fillRect(54, playerY + 19, 1, 1, GRAY);
    tft.fillRect(53, playerY + 20, 1, 1, GRAY);
    tft.fillRect(57, playerY + 14, 13, 11, ORANGE);
    tft.fillRect(57, playerY + 25, 5, 6, ORANGE);
    tft.fillRect(57, playerY + 31, 4, 1, ORANGE);
    tft.fillRect(58, playerY + 32, 3, 1, ORANGE);
    tft.fillRect(57, playerY + 33, 3, 2, ORANGE);
    tft.fillRect(58, playerY + 33, 1, 1, GRAY);
    tft.fillRect(57, playerY + 34, 1, 1, GRAY);
    tft.fillRect(59, playerY + 34, 1, 1, GRAY);
    tft.fillRect(63, playerY + 25, 5, 6, ORANGE);
    tft.fillRect(64, playerY + 31, 4, 2, ORANGE);
    tft.fillRect(65, playerY + 33, 4, 2, ORANGE);
    tft.fillRect(67, playerY + 33, 1, 1, GRAY);
    tft.fillRect(66, playerY + 34, 1, 1, GRAY);
    tft.fillRect(68, playerY + 34, 1, 1, GRAY);
    tft.fillRect(70, playerY + 14, 2, 5, ORANGE);
    tft.fillRect(72, playerY + 16, 1, 1, ORANGE);
    tft.fillRect(72, playerY + 17, 2, 3, ORANGE);
    tft.fillRect(73, playerY + 18, 2, 3, ORANGE);
    tft.fillRect(73, playerY + 19, 1, 1, GRAY);
    tft.fillRect(74, playerY + 20, 1, 1, GRAY);
    tft.fillRect(70, playerY + 13, 5, 10, BLUE);
    tft.fillRect(70, playerY + 14, 1, 4, ORANGE);
    tft.fillRect(71, playerY + 13, 1, 4, ORANGE);
    tft.fillRect(72, playerY + 13, 1, 3, ORANGE);
    tft.fillRect(73, playerY + 12, 1, 3, ORANGE);
    tft.fillRect(74, playerY + 11, 1, 3, ORANGE);
    tft.fillRect(65, playerY + 25, 7, 8, BLUE);
    tft.fillRect(65, playerY + 25, 6, 4, ORANGE);
    tft.fillRect(66, playerY + 29, 6, 1, ORANGE);
    tft.fillRect(67, playerY + 30, 5, 1, ORANGE);
    tft.fillRect(68, playerY + 31, 5, 1, ORANGE);
    tft.fillRect(69, playerY + 32, 4, 1, ORANGE);
    tft.fillRect(50, playerY + 14, 2, 8, BLUE);
    tft.fillRect(50, playerY + 24, 2, 11, ORANGE);
    tft.fillRect(52, playerY + 25, 24, 10, BLUE);
    tft.fillRect(56, playerY + 27, 4, 2, ORANGE);
    tft.fillRect(57, playerY + 25, 5, 2, ORANGE);
    tft.fillRect(65, playerY + 25, 5, 2, ORANGE);
    tft.fillRect(67, playerY + 27, 5, 2, ORANGE);
    tft.fillRect(50, playerY, 2, 14, BLUE);
    tft.fillRect(60, playerY, 5, 5, BLUE);
    tft.fillRect(60, playerY+2,5,3, BLUE);
    tft.fillRect(59, playerY+1, 8,1,BLUE);
    tft.fillRect(65,playerY+2, 1, 1, BLUE);
    tft.fillRect(58,playerY,13,1,BLUE);
    break;
  default:
    tft.fillRect(50, playerY, 25, 35, GREEN);
  }

  if (playerY < 210)
  {
    // Serial.println(playerY);
    tft.fillRect(50, playerY + 35, 25, 210 - (playerY), BLUE);
  }
  if (goingDown)
  {
    tft.fillRect(50, playerY - 35, 25, 35, BLUE);
  }
  delay(30);
}

unsigned long drawCloud()
{
  if (!isCloudPresent)
  {
    cloudY = rand() % 60 + 60;
    int diff = rand() % 2;
    cloud2Y = (diff) ? cloudY + 20 : cloudY - 20;
    if (cloud2Y >= 120)
      cloud2Y = 120;
    isCloudPresent = true;
  }

  tft.fillCircle(cloudX, cloudY, 10, WHITE);
  tft.fillCircle(cloudX + 10, cloudY, 10, WHITE);
  tft.fillCircle(cloudX + 20, cloudY, 10, WHITE);
  tft.fillCircle(cloudX + 8, cloudY - 10, 10, WHITE);
  tft.fillCircle(cloudX + 18, cloudY - 10, 10, WHITE);
  tft.fillRect(cloudX + 30, cloudY - 20, 35, 40, BLUE);

  tft.fillCircle(cloudX + 70, cloud2Y, 10, WHITE);
  tft.fillCircle(cloudX + 80, cloud2Y, 10, WHITE);
  tft.fillCircle(cloudX + 90, cloud2Y, 10, WHITE);
  tft.fillCircle(cloudX + 78, cloud2Y - 10, 10, WHITE);
  tft.fillCircle(cloudX + 88, cloud2Y - 10, 10, WHITE);
  tft.fillRect(cloudX + 100, cloud2Y - 20, 35, 40, BLUE);

  if (cloudX < -100)
  {
    cloudX = 240;
    isCloudPresent = 0;
  }
}

unsigned long sendObstacle()
{
  if (!isObstaclePresent)
  {
    obstacleW = rand() % 25 + 10;
    obstacleH = rand() % 30 + 10;
    isObstaclePresent = true;
  }

  tft.fillRect(obstacleX + obstacleW / 2 - 2, 250 - obstacleH, 4, obstacleH - 5, TRINK);
  tft.fillRect(obstacleX, 246 - obstacleH, obstacleW, 4, LGREEN);
  tft.fillRect(obstacleX + 5, 245 - obstacleH, obstacleW - 7, 1, LGREEN);

  tft.fillRect(obstacleX + obstacleW, 245 - obstacleH, obstacleW + 20, obstacleH, BLUE);

  if (obstacleX < -50)
  {
    obstacleX = 240;
    isObstaclePresent = false;
  }
}

unsigned long sendYarn()
{
  if (!isYarnPresent)
    isYarnPresent = true;

  if (yarnDeflect)
  {
    tft.fillCircle(yarnX, yarnY, 10, YELLOW);
    tft.fillCircle(yarnX, yarnY + 20, 10, BLUE);
  }
  else
  {
    tft.fillCircle(yarnX, yarnY, 10, YELLOW);
    tft.fillCircle(yarnX + 15, yarnY - 15, 10, BLUE);
  }
  if (yarnY < -20)
  {
    yarnDeflect = false;
    isYarnPresent = false;
    yarnX = 300;
    yarnY = 0;
  }
}

unsigned long displayScore()
{
  tft.fillRect(0, 5, 240, 30, BLUE);
  score += 10;
  tft.setCursor(5, 5);
  tft.setTextSize(2);
  tft.println(score);
}

unsigned long jump()
{
  if (momentum > -60 && !isPressed && !goingDown)
  {
    if (!isJumpPlaying)
      jumpSound();
    momentum -= 20;
  }
  else if (momentum < 0)
  {
    isPressed = true;
    goingDown = true;
    momentum += 20;
  }
  playerY = 210 + momentum;
}

unsigned long collisionCheck()
{
  if ((obstacleX >= 50 && obstacleX <= 75) || (obstacleX + obstacleW >= 50 && obstacleX + obstacleW <= 75))
  {
    if (245 - obstacleH >= playerY && 245 - obstacleH <= playerY + 35)
    {
      deathSound();
      delay(1000);
      isGameOver = true;
    }
  }
  else if (yarnX <= 75 && yarnY >= playerY)
  {
    deathSound();
    delay(1000);
    isGameOver = true;
  }
}

unsigned long checkAccel()
{

  if (quakeSequence > -1)
  {
    yarnDeflect = true;
    switch (quakeSequence)
    {
    case 3:
      quakePlayerY = playerY;
      tft.drawCircle(60, quakePlayerY, 25, RED);
      break;
    case 2:
      tft.drawCircle(60, quakePlayerY, 25, BLUE);
      tft.fillRect(0, 245, 150, 75, GREEN);
      tft.drawCircle(60, quakePlayerY, 50, RED);
      break;
    case 1:
      tft.drawCircle(60, quakePlayerY, 50, BLUE);
      tft.fillRect(0, 245, 150, 75, GREEN);
      tft.drawCircle(60, quakePlayerY, 75, RED);
      break;
    case 0:
      tft.drawCircle(60, quakePlayerY, 75, BLUE);
      tft.fillRect(0, 245, 150, 75, GREEN);
      break;
    }
    delay(50);
    quakeSequence--;
  }
  else
  {

    int x = analogRead(ACCELX);
    int y = analogRead(ACCELY);
    int z = analogRead(ACCELZ);

    long xMilliG = map(x, xMin, xMax, -1000, 1000);
    long yMilliG = map(y, yMin, yMax, -1000, 1000);
    long zMilliG = map(z, zMin, zMax, -1000, 1000);
    //--------------------------------------------------------------
    // re-scale to fractional Gs
    float x_g_value = xMilliG / 1000.0;
    float y_g_value = yMilliG / 1000.0;
    float z_g_value = zMilliG / 1000.0;

    float accelerationMagnitude = sqrt(x_g_value * x_g_value + y_g_value * y_g_value + z_g_value * z_g_value);
    if (accelerationMagnitude > ACCELEROMETER_THRESHOLD)
    {
      quakeSequence = 3;
    }
  }
}
// death animation commences

unsigned long gameOver(int s)
{
  switch (gameOverSequence)
  {
  case 5:
    tft.fillRect(58, 219 - 20, 12, 4, ORANGE);
    tft.fillRect(56, 218 - 20, 2, 6, ORANGE);
    tft.fillRect(67, 215 - 20, 3, 3, ORANGE);
    tft.fillRect(68, 216 - 20, 3, 5, ORANGE);
    tft.fillRect(57, 210 - 20, 1, 1, ORANGE);
    tft.fillRect(56, 211 - 20, 3, 1, ORANGE);
    tft.fillRect(55, 212 - 20, 5, 1, ORANGE);
    tft.fillRect(55, 213 - 20, 2, 1, ORANGE);
    tft.fillRect(57, 213 - 20, 1, 1, BROWN);
    tft.fillRect(58, 213 - 20, 2, 1, ORANGE);
    tft.fillRect(55, 214 - 20, 1, 1, ORANGE);
    tft.fillRect(56, 214 - 20, 3, 1, BROWN);
    tft.fillRect(59, 214 - 20, 1, 1, ORANGE);
    tft.fillRect(55, 215 - 20, 14, 4, ORANGE);
    tft.fillRect(70, 216 - 20, 1, 1, ORANGE);
    tft.fillRect(67, 211 - 20, 2, 1, ORANGE);
    tft.fillRect(66, 212 - 20, 2, 1, ORANGE);
    tft.fillRect(65, 213 - 20, 5, 2, ORANGE);
    tft.fillRect(67, 215 - 20, 3, 2, BROWN);
    tft.fillRect(68, 212 - 20, 1, 1, BROWN);
    tft.fillRect(69, 211 - 20, 1, 2, ORANGE);
    tft.fillRect(55, 215 - 20, 14, 5, ORANGE);
    tft.fillRect(70, 216 - 20, 1, 13, ORANGE);
    tft.fillRect(71, 217 - 20, 1, 5, ORANGE);
    tft.fillRect(72, 217 - 20, 1, 1, GRAY);
    tft.fillRect(72, 219 - 20, 2, 1, GRAY);
    tft.fillRect(60, 216 - 20, 2, 2, WHITE);
    tft.fillRect(61, 217 - 20, 1, 1, BLACK);
    tft.fillRect(65, 216 - 20, 2, 2, WHITE);
    tft.fillRect(66, 217 - 20, 1, 1, BLACK);
    tft.fillRect(62, 219 - 20, 1, 1, PINK);
    tft.fillRect(61, 220 - 20, 1, 1, BLACK);
    tft.fillRect(62, 221 - 20, 1, 1, BLACK);
    tft.fillRect(63, 220 - 20, 1, 1, BLACK);
    tft.fillRect(64, 221 - 20, 2, 1, BLACK);
    tft.fillRect(66, 220 - 20, 1, 1, BLACK);
    tft.fillRect(54, 217 - 20, 3, 1, GRAY);
    tft.fillRect(54, 219 - 20, 2, 1, GRAY);
    tft.fillRect(56, 221 - 20, 1, 1, GRAY);
    tft.fillRect(57, 222 - 20, 1, 1, GRAY);
    tft.fillRect(58, 223 - 20, 12, 1, GRAY);
    tft.fillRect(70, 222 - 20, 1, 1, GRAY);
    tft.fillRect(71, 221 - 20, 1, 1, GRAY);
    tft.fillRect(70, 223 - 20, 2, 1, ORANGE);
    tft.fillRect(54, 223 - 20, 4, 1, ORANGE);
    tft.fillRect(52, 232 - 20, 2, 2, ORANGE);
    tft.fillRect(54, 233 - 20, 1, 2, ORANGE);
    tft.fillRect(55, 233 - 20, 2, 2, ORANGE);
    tft.fillRect(54, 222 - 20, 3, 7, ORANGE);
    tft.fillRect(53, 229 - 20, 2, 2, ORANGE);
    tft.fillRect(54, 229 - 20, 1, 1, GRAY);
    tft.fillRect(53, 230 - 20, 1, 1, GRAY);
    tft.fillRect(57, 224 - 20, 13, 11, ORANGE);
    tft.fillRect(57, 235 - 20, 5, 6, ORANGE);
    tft.fillRect(57, 241 - 20, 4, 1, ORANGE);
    tft.fillRect(58, 242 - 20, 3, 1, ORANGE);
    tft.fillRect(57, 243 - 20, 3, 2, ORANGE);
    tft.fillRect(58, 243 - 20, 1, 1, GRAY);
    tft.fillRect(57, 244 - 20, 1, 1, GRAY);
    tft.fillRect(59, 244 - 20, 1, 1, GRAY);
    tft.fillRect(63, 235 - 20, 5, 6, ORANGE);
    tft.fillRect(64, 241 - 20, 4, 2, ORANGE);
    tft.fillRect(65, 243 - 20, 4, 2, ORANGE);
    tft.fillRect(67, 243 - 20, 1, 1, GRAY);
    tft.fillRect(66, 244 - 20, 1, 1, GRAY);
    tft.fillRect(68, 244 - 20, 1, 1, GRAY);
    tft.fillRect(70, 224 - 20, 2, 5, ORANGE);
    tft.fillRect(72, 226 - 20, 1, 1, ORANGE);
    tft.fillRect(72, 227 - 20, 2, 3, ORANGE);
    tft.fillRect(73, 228 - 20, 2, 3, ORANGE);
    tft.fillRect(73, 229 - 20, 1, 1, GRAY);
    tft.fillRect(74, 230 - 20, 1, 1, GRAY);
    tft.fillRect(59, 216 - 20, 10, 2, ORANGE);
    tft.fillRect(59, 216 - 20, 1, 1, BLACK);
    tft.fillRect(60, 217 - 20, 1, 1, BLACK);
    tft.fillRect(59, 218 - 20, 1, 1, BLACK);
    tft.fillRect(62, 216 - 20, 1, 1, BLACK);
    tft.fillRect(62, 218 - 20, 1, 1, BLACK);
    tft.fillRect(65, 216 - 20, 1, 1, BLACK);
    tft.fillRect(67, 217 - 20, 1, 1, BLACK);
    tft.fillRect(55, 218 - 20, 1, 1, BLACK);
    tft.fillRect(68, 216 - 20, 1, 1, BLACK);
    tft.fillRect(68, 218 - 20, 1, 1, BLACK);
    tft.fillRect(50, 210 - 20 + 35, 25, 20, BLUE);

    break;
  case 4:
    tft.fillRect(58, 219 + 10, 12, 4, ORANGE);
    tft.fillRect(56, 218 + 10, 2, 6, ORANGE);
    tft.fillRect(67, 215 + 10, 3, 3, ORANGE);
    tft.fillRect(68, 216 + 10, 3, 5, ORANGE);
    tft.fillRect(57, 210 + 10, 1, 1, ORANGE);
    tft.fillRect(56, 211 + 10, 3, 1, ORANGE);
    tft.fillRect(55, 212 + 10, 5, 1, ORANGE);
    tft.fillRect(55, 213 + 10, 2, 1, ORANGE);
    tft.fillRect(57, 213 + 10, 1, 1, BROWN);
    tft.fillRect(58, 213 + 10, 2, 1, ORANGE);
    tft.fillRect(55, 214 + 10, 1, 1, ORANGE);
    tft.fillRect(56, 214 + 10, 3, 1, BROWN);
    tft.fillRect(59, 214 + 10, 1, 1, ORANGE);
    tft.fillRect(55, 215 + 10, 14, 4, ORANGE);
    tft.fillRect(70, 216 + 10, 1, 1, ORANGE);
    tft.fillRect(67, 211 + 10, 2, 1, ORANGE);
    tft.fillRect(66, 212 + 10, 2, 1, ORANGE);
    tft.fillRect(65, 213 + 10, 5, 2, ORANGE);
    tft.fillRect(67, 215 + 10, 3, 2, BROWN);
    tft.fillRect(68, 212 + 10, 1, 1, BROWN);
    tft.fillRect(69, 211 + 10, 1, 2, ORANGE);
    tft.fillRect(55, 215 + 10, 14, 5, ORANGE);
    tft.fillRect(70, 216 + 10, 1, 13, ORANGE);
    tft.fillRect(71, 217 + 10, 1, 5, ORANGE);
    tft.fillRect(72, 217 + 10, 1, 1, GRAY);
    tft.fillRect(72, 219 + 10, 2, 1, GRAY);
    tft.fillRect(60, 216 + 10, 2, 2, WHITE);
    tft.fillRect(61, 217 + 10, 1, 1, BLACK);
    tft.fillRect(65, 216 + 10, 2, 2, WHITE);
    tft.fillRect(66, 217 + 10, 1, 1, BLACK);
    tft.fillRect(62, 219 + 10, 1, 1, PINK);
    tft.fillRect(61, 220 + 10, 1, 1, BLACK);
    tft.fillRect(62, 221 + 10, 1, 1, BLACK);
    tft.fillRect(63, 220 + 10, 1, 1, BLACK);
    tft.fillRect(64, 221 + 10, 2, 1, BLACK);
    tft.fillRect(66, 220 + 10, 1, 1, BLACK);
    tft.fillRect(54, 217 + 10, 3, 1, GRAY);
    tft.fillRect(54, 219 + 10, 2, 1, GRAY);
    tft.fillRect(56, 221 + 10, 1, 1, GRAY);
    tft.fillRect(57, 222 + 10, 1, 1, GRAY);
    tft.fillRect(58, 223 + 10, 12, 1, GRAY);
    tft.fillRect(70, 222 + 10, 1, 1, GRAY);
    tft.fillRect(71, 221 + 10, 1, 1, GRAY);
    tft.fillRect(70, 223 + 10, 2, 1, ORANGE);
    tft.fillRect(54, 223 + 10, 4, 1, ORANGE);
    tft.fillRect(52, 232 + 10, 2, 2, ORANGE);
    tft.fillRect(54, 233 + 10, 1, 2, ORANGE);
    tft.fillRect(55, 233 + 10, 2, 2, ORANGE);
    tft.fillRect(54, 222 + 10, 3, 7, ORANGE);
    tft.fillRect(53, 229 + 10, 2, 2, ORANGE);
    tft.fillRect(54, 229 + 10, 1, 1, GRAY);
    tft.fillRect(53, 230 + 10, 1, 1, GRAY);
    tft.fillRect(57, 224 + 10, 13, 11, ORANGE);
    tft.fillRect(57, 235 + 10, 5, 6, ORANGE);
    tft.fillRect(57, 241 + 10, 4, 1, ORANGE);
    tft.fillRect(58, 242 + 10, 3, 1, ORANGE);
    tft.fillRect(57, 243 + 10, 3, 2, ORANGE);
    tft.fillRect(58, 243 + 10, 1, 1, GRAY);
    tft.fillRect(57, 244 + 10, 1, 1, GRAY);
    tft.fillRect(59, 244 + 10, 1, 1, GRAY);
    tft.fillRect(63, 235 + 10, 5, 6, ORANGE);
    tft.fillRect(64, 241 + 10, 4, 2, ORANGE);
    tft.fillRect(65, 243 + 10, 4, 2, ORANGE);
    tft.fillRect(67, 243 + 10, 1, 1, GRAY);
    tft.fillRect(66, 244 + 10, 1, 1, GRAY);
    tft.fillRect(68, 244 + 10, 1, 1, GRAY);
    tft.fillRect(70, 224 + 10, 2, 5, ORANGE);
    tft.fillRect(72, 226 + 10, 1, 1, ORANGE);
    tft.fillRect(72, 227 + 10, 2, 3, ORANGE);
    tft.fillRect(73, 228 + 10, 2, 3, ORANGE);
    tft.fillRect(73, 229 + 10, 1, 1, GRAY);
    tft.fillRect(74, 230 + 10, 1, 1, GRAY);
    tft.fillRect(59, 216 + 10, 10, 2, ORANGE);
    tft.fillRect(59, 216 + 10, 1, 1, BLACK);
    tft.fillRect(60, 217 + 10, 1, 1, BLACK);
    tft.fillRect(59, 218 + 10, 1, 1, BLACK);
    tft.fillRect(62, 216 + 10, 1, 1, BLACK);
    tft.fillRect(62, 218 + 10, 1, 1, BLACK);
    tft.fillRect(65, 216 + 10, 1, 1, BLACK);
    tft.fillRect(67, 217 + 10, 1, 1, BLACK);
    tft.fillRect(55, 218 + 10, 1, 1, BLACK);
    tft.fillRect(68, 216 + 10, 1, 1, BLACK);
    tft.fillRect(68, 218 + 10, 1, 1, BLACK);
    tft.fillRect(50, 210 - 20, 35, 30, BLUE);
    sendObstacle();

    break;
  case 3:
    tft.fillRect(58, 219 + 40, 12, 4, ORANGE);
    tft.fillRect(56, 218 + 40, 2, 6, ORANGE);
    tft.fillRect(67, 215 + 40, 3, 3, ORANGE);
    tft.fillRect(68, 216 + 40, 3, 5, ORANGE);
    tft.fillRect(57, 210 + 40, 1, 1, ORANGE);
    tft.fillRect(56, 211 + 40, 3, 1, ORANGE);
    tft.fillRect(55, 212 + 40, 5, 1, ORANGE);
    tft.fillRect(55, 213 + 40, 2, 1, ORANGE);
    tft.fillRect(57, 213 + 40, 1, 1, BROWN);
    tft.fillRect(58, 213 + 40, 2, 1, ORANGE);
    tft.fillRect(55, 214 + 40, 1, 1, ORANGE);
    tft.fillRect(56, 214 + 40, 3, 1, BROWN);
    tft.fillRect(59, 214 + 40, 1, 1, ORANGE);
    tft.fillRect(55, 215 + 40, 14, 4, ORANGE);
    tft.fillRect(70, 216 + 40, 1, 1, ORANGE);
    tft.fillRect(67, 211 + 40, 2, 1, ORANGE);
    tft.fillRect(66, 212 + 40, 2, 1, ORANGE);
    tft.fillRect(65, 213 + 40, 5, 2, ORANGE);
    tft.fillRect(67, 215 + 40, 3, 2, BROWN);
    tft.fillRect(68, 212 + 40, 1, 1, BROWN);
    tft.fillRect(69, 211 + 40, 1, 2, ORANGE);
    tft.fillRect(55, 215 + 40, 14, 5, ORANGE);
    tft.fillRect(70, 216 + 40, 1, 13, ORANGE);
    tft.fillRect(71, 217 + 40, 1, 5, ORANGE);
    tft.fillRect(72, 217 + 40, 1, 1, GRAY);
    tft.fillRect(72, 219 + 40, 2, 1, GRAY);
    tft.fillRect(60, 216 + 40, 2, 2, WHITE);
    tft.fillRect(61, 217 + 40, 1, 1, BLACK);
    tft.fillRect(65, 216 + 40, 2, 2, WHITE);
    tft.fillRect(66, 217 + 40, 1, 1, BLACK);
    tft.fillRect(62, 219 + 40, 1, 1, PINK);
    tft.fillRect(61, 220 + 40, 1, 1, BLACK);
    tft.fillRect(62, 221 + 40, 1, 1, BLACK);
    tft.fillRect(63, 220 + 40, 1, 1, BLACK);
    tft.fillRect(64, 221 + 40, 2, 1, BLACK);
    tft.fillRect(66, 220 + 40, 1, 1, BLACK);
    tft.fillRect(54, 217 + 40, 3, 1, GRAY);
    tft.fillRect(54, 219 + 40, 2, 1, GRAY);
    tft.fillRect(56, 221 + 40, 1, 1, GRAY);
    tft.fillRect(57, 222 + 40, 1, 1, GRAY);
    tft.fillRect(58, 223 + 40, 12, 1, GRAY);
    tft.fillRect(70, 222 + 40, 1, 1, GRAY);
    tft.fillRect(71, 221 + 40, 1, 1, GRAY);
    tft.fillRect(70, 223 + 40, 2, 1, ORANGE);
    tft.fillRect(54, 223 + 40, 4, 1, ORANGE);
    tft.fillRect(52, 232 + 40, 2, 2, ORANGE);
    tft.fillRect(54, 233 + 40, 1, 2, ORANGE);
    tft.fillRect(55, 233 + 40, 2, 2, ORANGE);
    tft.fillRect(54, 222 + 40, 3, 7, ORANGE);
    tft.fillRect(53, 229 + 40, 2, 2, ORANGE);
    tft.fillRect(54, 229 + 40, 1, 1, GRAY);
    tft.fillRect(53, 230 + 40, 1, 1, GRAY);
    tft.fillRect(57, 224 + 40, 13, 11, ORANGE);
    tft.fillRect(57, 235 + 40, 5, 6, ORANGE);
    tft.fillRect(57, 241 + 40, 4, 1, ORANGE);
    tft.fillRect(58, 242 + 40, 3, 1, ORANGE);
    tft.fillRect(57, 243 + 40, 3, 2, ORANGE);
    tft.fillRect(58, 243 + 40, 1, 1, GRAY);
    tft.fillRect(57, 244 + 40, 1, 1, GRAY);
    tft.fillRect(59, 244 + 40, 1, 1, GRAY);
    tft.fillRect(63, 235 + 40, 5, 6, ORANGE);
    tft.fillRect(64, 241 + 40, 4, 2, ORANGE);
    tft.fillRect(65, 243 + 40, 4, 2, ORANGE);
    tft.fillRect(67, 243 + 40, 1, 1, GRAY);
    tft.fillRect(66, 244 + 40, 1, 1, GRAY);
    tft.fillRect(68, 244 + 40, 1, 1, GRAY);
    tft.fillRect(70, 224 + 40, 2, 5, ORANGE);
    tft.fillRect(72, 226 + 40, 1, 1, ORANGE);
    tft.fillRect(72, 227 + 40, 2, 3, ORANGE);
    tft.fillRect(73, 228 + 40, 2, 3, ORANGE);
    tft.fillRect(73, 229 + 40, 1, 1, GRAY);
    tft.fillRect(74, 230 + 40, 1, 1, GRAY);
    tft.fillRect(59, 216 + 40, 10, 2, ORANGE);
    tft.fillRect(59, 216 + 40, 1, 1, BLACK);
    tft.fillRect(60, 217 + 40, 1, 1, BLACK);
    tft.fillRect(59, 218 + 40, 1, 1, BLACK);
    tft.fillRect(62, 216 + 40, 1, 1, BLACK);
    tft.fillRect(62, 218 + 40, 1, 1, BLACK);
    tft.fillRect(65, 216 + 40, 1, 1, BLACK);
    tft.fillRect(67, 217 + 40, 1, 1, BLACK);
    tft.fillRect(55, 218 + 40, 1, 1, BLACK);
    tft.fillRect(68, 216 + 40, 1, 1, BLACK);
    tft.fillRect(68, 218 + 40, 1, 1, BLACK);
    tft.fillRect(50, 210 + 10, 35, 245 - 210, BLUE);
    tft.fillRect(50, 245, 35, (245) - 210, GREEN);
    sendObstacle();

    break;
  case 2:
    tft.fillRect(58, 219 + 70, 12, 4, ORANGE);
    tft.fillRect(56, 218 + 70, 2, 6, ORANGE);
    tft.fillRect(67, 215 + 70, 3, 3, ORANGE);
    tft.fillRect(68, 216 + 70, 3, 5, ORANGE);
    tft.fillRect(57, 210 + 70, 1, 1, ORANGE);
    tft.fillRect(56, 211 + 70, 3, 1, ORANGE);
    tft.fillRect(55, 212 + 70, 5, 1, ORANGE);
    tft.fillRect(55, 213 + 70, 2, 1, ORANGE);
    tft.fillRect(57, 213 + 70, 1, 1, BROWN);
    tft.fillRect(58, 213 + 70, 2, 1, ORANGE);
    tft.fillRect(55, 214 + 70, 1, 1, ORANGE);
    tft.fillRect(56, 214 + 70, 3, 1, BROWN);
    tft.fillRect(59, 214 + 70, 1, 1, ORANGE);
    tft.fillRect(55, 215 + 70, 14, 4, ORANGE);
    tft.fillRect(70, 216 + 70, 1, 1, ORANGE);
    tft.fillRect(67, 211 + 70, 2, 1, ORANGE);
    tft.fillRect(66, 212 + 70, 2, 1, ORANGE);
    tft.fillRect(65, 213 + 70, 5, 2, ORANGE);
    tft.fillRect(67, 215 + 70, 3, 2, BROWN);
    tft.fillRect(68, 212 + 70, 1, 1, BROWN);
    tft.fillRect(69, 211 + 70, 1, 2, ORANGE);
    tft.fillRect(55, 215 + 70, 14, 5, ORANGE);
    tft.fillRect(70, 216 + 70, 1, 13, ORANGE);
    tft.fillRect(71, 217 + 70, 1, 5, ORANGE);
    tft.fillRect(72, 217 + 70, 1, 1, GRAY);
    tft.fillRect(72, 219 + 70, 2, 1, GRAY);
    tft.fillRect(60, 216 + 70, 2, 2, WHITE);
    tft.fillRect(61, 217 + 70, 1, 1, BLACK);
    tft.fillRect(65, 216 + 70, 2, 2, WHITE);
    tft.fillRect(66, 217 + 70, 1, 1, BLACK);
    tft.fillRect(62, 219 + 70, 1, 1, PINK);
    tft.fillRect(61, 220 + 70, 1, 1, BLACK);
    tft.fillRect(62, 221 + 70, 1, 1, BLACK);
    tft.fillRect(63, 220 + 70, 1, 1, BLACK);
    tft.fillRect(64, 221 + 70, 2, 1, BLACK);
    tft.fillRect(66, 220 + 70, 1, 1, BLACK);
    tft.fillRect(54, 217 + 70, 3, 1, GRAY);
    tft.fillRect(54, 219 + 70, 2, 1, GRAY);
    tft.fillRect(56, 221 + 70, 1, 1, GRAY);
    tft.fillRect(57, 222 + 70, 1, 1, GRAY);
    tft.fillRect(58, 223 + 70, 12, 1, GRAY);
    tft.fillRect(70, 222 + 70, 1, 1, GRAY);
    tft.fillRect(71, 221 + 70, 1, 1, GRAY);
    tft.fillRect(70, 223 + 70, 2, 1, ORANGE);
    tft.fillRect(54, 223 + 70, 4, 1, ORANGE);
    tft.fillRect(52, 232 + 70, 2, 2, ORANGE);
    tft.fillRect(54, 233 + 70, 1, 2, ORANGE);
    tft.fillRect(55, 233 + 70, 2, 2, ORANGE);
    tft.fillRect(54, 222 + 70, 3, 7, ORANGE);
    tft.fillRect(53, 229 + 70, 2, 2, ORANGE);
    tft.fillRect(54, 229 + 70, 1, 1, GRAY);
    tft.fillRect(53, 230 + 70, 1, 1, GRAY);
    tft.fillRect(57, 224 + 70, 13, 11, ORANGE);
    tft.fillRect(57, 235 + 70, 5, 6, ORANGE);
    tft.fillRect(57, 241 + 70, 4, 1, ORANGE);
    tft.fillRect(58, 242 + 70, 3, 1, ORANGE);
    tft.fillRect(57, 243 + 70, 3, 2, ORANGE);
    tft.fillRect(58, 243 + 70, 1, 1, GRAY);
    tft.fillRect(57, 244 + 70, 1, 1, GRAY);
    tft.fillRect(59, 244 + 70, 1, 1, GRAY);
    tft.fillRect(63, 235 + 70, 5, 6, ORANGE);
    tft.fillRect(64, 241 + 70, 4, 2, ORANGE);
    tft.fillRect(65, 243 + 70, 4, 2, ORANGE);
    tft.fillRect(67, 243 + 70, 1, 1, GRAY);
    tft.fillRect(66, 244 + 70, 1, 1, GRAY);
    tft.fillRect(68, 244 + 70, 1, 1, GRAY);
    tft.fillRect(70, 224 + 70, 2, 5, ORANGE);
    tft.fillRect(72, 226 + 70, 1, 1, ORANGE);
    tft.fillRect(72, 227 + 70, 2, 3, ORANGE);
    tft.fillRect(73, 228 + 70, 2, 3, ORANGE);
    tft.fillRect(73, 229 + 70, 1, 1, GRAY);
    tft.fillRect(74, 230 + 70, 1, 1, GRAY);
    tft.fillRect(59, 216 + 70, 10, 2, ORANGE);
    tft.fillRect(59, 216 + 70, 1, 1, BLACK);
    tft.fillRect(60, 217 + 70, 1, 1, BLACK);
    tft.fillRect(59, 218 + 70, 1, 1, BLACK);
    tft.fillRect(62, 216 + 70, 1, 1, BLACK);
    tft.fillRect(62, 218 + 70, 1, 1, BLACK);
    tft.fillRect(65, 216 + 70, 1, 1, BLACK);
    tft.fillRect(67, 217 + 70, 1, 1, BLACK);
    tft.fillRect(55, 218 + 70, 1, 1, BLACK);
    tft.fillRect(68, 216 + 70, 1, 1, BLACK);
    tft.fillRect(68, 218 + 70, 1, 1, BLACK);
    tft.fillRect(50, 245, 35, (210 + 70) - 210, GREEN);
    sendObstacle();

    break;
  case 1:
    tft.fillRect(58, 219 + 90, 12, 4, ORANGE);
    tft.fillRect(56, 218 + 90, 2, 6, ORANGE);
    tft.fillRect(67, 215 + 90, 3, 3, ORANGE);
    tft.fillRect(68, 216 + 90, 3, 5, ORANGE);
    tft.fillRect(57, 210 + 90, 1, 1, ORANGE);
    tft.fillRect(56, 211 + 90, 3, 1, ORANGE);
    tft.fillRect(55, 212 + 90, 5, 1, ORANGE);
    tft.fillRect(55, 213 + 90, 2, 1, ORANGE);
    tft.fillRect(57, 213 + 90, 1, 1, BROWN);
    tft.fillRect(58, 213 + 90, 2, 1, ORANGE);
    tft.fillRect(55, 214 + 90, 1, 1, ORANGE);
    tft.fillRect(56, 214 + 90, 3, 1, BROWN);
    tft.fillRect(59, 214 + 90, 1, 1, ORANGE);
    tft.fillRect(55, 215 + 90, 14, 4, ORANGE);
    tft.fillRect(70, 216 + 90, 1, 1, ORANGE);
    tft.fillRect(67, 211 + 90, 2, 1, ORANGE);
    tft.fillRect(66, 212 + 90, 2, 1, ORANGE);
    tft.fillRect(65, 213 + 90, 5, 2, ORANGE);
    tft.fillRect(67, 215 + 90, 3, 2, BROWN);
    tft.fillRect(68, 212 + 90, 1, 1, BROWN);
    tft.fillRect(69, 211 + 90, 1, 2, ORANGE);
    tft.fillRect(55, 215 + 90, 14, 5, ORANGE);
    tft.fillRect(70, 216 + 90, 1, 13, ORANGE);
    tft.fillRect(71, 217 + 90, 1, 5, ORANGE);
    tft.fillRect(72, 217 + 90, 1, 1, GRAY);
    tft.fillRect(72, 219 + 90, 2, 1, GRAY);
    tft.fillRect(60, 216 + 90, 2, 2, WHITE);
    tft.fillRect(61, 217 + 90, 1, 1, BLACK);
    tft.fillRect(65, 216 + 90, 2, 2, WHITE);
    tft.fillRect(66, 217 + 90, 1, 1, BLACK);
    tft.fillRect(62, 219 + 90, 1, 1, PINK);
    tft.fillRect(61, 220 + 90, 1, 1, BLACK);
    tft.fillRect(62, 221 + 90, 1, 1, BLACK);
    tft.fillRect(63, 220 + 90, 1, 1, BLACK);
    tft.fillRect(64, 221 + 90, 2, 1, BLACK);
    tft.fillRect(66, 220 + 90, 1, 1, BLACK);
    tft.fillRect(54, 217 + 90, 3, 1, GRAY);
    tft.fillRect(54, 219 + 90, 2, 1, GRAY);
    tft.fillRect(56, 221 + 90, 1, 1, GRAY);
    tft.fillRect(57, 222 + 90, 1, 1, GRAY);
    tft.fillRect(58, 223 + 90, 12, 1, GRAY);
    tft.fillRect(70, 222 + 90, 1, 1, GRAY);
    tft.fillRect(71, 221 + 90, 1, 1, GRAY);
    tft.fillRect(70, 223 + 90, 2, 1, ORANGE);
    tft.fillRect(54, 223 + 90, 4, 1, ORANGE);
    tft.fillRect(52, 232 + 90, 2, 2, ORANGE);
    tft.fillRect(54, 233 + 90, 1, 2, ORANGE);
    tft.fillRect(55, 233 + 90, 2, 2, ORANGE);
    tft.fillRect(54, 222 + 90, 3, 7, ORANGE);
    tft.fillRect(53, 229 + 90, 2, 2, ORANGE);
    tft.fillRect(54, 229 + 90, 1, 1, GRAY);
    tft.fillRect(53, 230 + 90, 1, 1, GRAY);
    tft.fillRect(57, 224 + 90, 13, 11, ORANGE);
    tft.fillRect(57, 235 + 90, 5, 6, ORANGE);
    tft.fillRect(57, 241 + 90, 4, 1, ORANGE);
    tft.fillRect(58, 242 + 90, 3, 1, ORANGE);
    tft.fillRect(57, 243 + 90, 3, 2, ORANGE);
    tft.fillRect(58, 243 + 90, 1, 1, GRAY);
    tft.fillRect(57, 244 + 90, 1, 1, GRAY);
    tft.fillRect(59, 244 + 90, 1, 1, GRAY);
    tft.fillRect(63, 235 + 90, 5, 6, ORANGE);
    tft.fillRect(64, 241 + 90, 4, 2, ORANGE);
    tft.fillRect(65, 243 + 90, 4, 2, ORANGE);
    tft.fillRect(67, 243 + 90, 1, 1, GRAY);
    tft.fillRect(66, 244 + 90, 1, 1, GRAY);
    tft.fillRect(68, 244 + 90, 1, 1, GRAY);
    tft.fillRect(70, 224 + 90, 2, 5, ORANGE);
    tft.fillRect(72, 226 + 90, 1, 1, ORANGE);
    tft.fillRect(72, 227 + 90, 2, 3, ORANGE);
    tft.fillRect(73, 228 + 90, 2, 3, ORANGE);
    tft.fillRect(73, 229 + 90, 1, 1, GRAY);
    tft.fillRect(74, 230 + 90, 1, 1, GRAY);
    tft.fillRect(59, 216 + 90, 10, 2, ORANGE);
    tft.fillRect(59, 216 + 90, 1, 1, BLACK);
    tft.fillRect(60, 217 + 90, 1, 1, BLACK);
    tft.fillRect(59, 218 + 90, 1, 1, BLACK);
    tft.fillRect(62, 216 + 90, 1, 1, BLACK);
    tft.fillRect(62, 218 + 90, 1, 1, BLACK);
    tft.fillRect(65, 216 + 90, 1, 1, BLACK);
    tft.fillRect(67, 217 + 90, 1, 1, BLACK);
    tft.fillRect(55, 218 + 90, 1, 1, BLACK);
    tft.fillRect(68, 216 + 90, 1, 1, BLACK);
    tft.fillRect(68, 218 + 90, 1, 1, BLACK);
    tft.fillRect(50, 245, 35, (playerY + 90) - 210, GREEN);
    break;
  case 0:
    tft.setCursor(60, 100);
    tft.println("GAME OVER");
    tft.setCursor(60, 120);
    tft.println("SCORE:");
    tft.setCursor(60, 140);
    tft.println(s);
    delay(1000);
    tft.setCursor(60, 160);
    tft.println("PRESS BUTTON");
    tft.setCursor(60, 180);
    tft.println("TO RESTART");
    break;
  }
}

unsigned long jumpSound()
{
  isJumpPlaying = true;
    int tone1 = 14;
    tone(tone1, 391, 47.619047619);
    // delay(47.619047619);
    // delay(47.619047619);
    // delay(23.8095238095);
    tone(tone1, 415, 47.619047619);
    // delay(47.619047619);
    tone(tone1, 880, 47.619047619);
    // delay(47.619047619);
    // delay(23.8095238095);
    // delay(23.8095238095);
    tone(tone1,1046, 23.8095238095);
    // delay(23.8095238095);
    tone(tone1, 554, 23.8095238095);
    // delay(23.8095238095);
    // delay(23.8095238095);
    tone(tone1, 587, 23.8095238095);
    // delay(23.8095238095);
    tone(tone1, 622, 23.8095238095);
    // delay(23.8095238095);
    tone(tone1, 659, 47.619047619);
    // delay(47.619047619);
    isJumpPlaying = false;
}
unsigned long deathSound()
{
  int tone1 = 14;

  tone(tone1, NOTE_B3, 250);
  delay(250);
  tone(tone1,NOTE_F4, 250);
  delay(500);
  tone(tone1,NOTE_F4, 250);
  delay(250);
  tone(tone1,NOTE_F4, 333);
  delay(333);
  tone(tone1,NOTE_E4, 333);
  delay(333);
  tone(tone1,NOTE_D4, 333);
  delay(333);
  tone(tone1,NOTE_C4, 250);
  delay(250);
  tone(tone1,NOTE_E3, 250);
  delay(250);
  delay(250);
  tone(tone1,NOTE_E3, 250);
  delay(250);
  tone(tone1,NOTE_C3, 500);
}
