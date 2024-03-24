// IMPORTANT: ELEGOO_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Elegoo_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <SPI.h>
#include <Tone.h>
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define BUTTON A6 //Jump button pin

#define ACCELX A8 //accelerometer xyz
#define ACCELY A9
#define ACCELZ A10


// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF



Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Elegoo_TFTLCD tft;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
   if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
  
  }

  tft.begin(identifier);

  Serial.println(F("Benchmark                Time (microseconds)"));
  tft.setRotation(2);
  Serial.print(F("Text"));
  Serial.println(titleScreen());
  delay(5000);
  tft.fillRect(0,0, 240, 245, BLUE);
  tft.fillRect(0, 245, 240, 75, GREEN);


  Serial.println(F("Done!"));
}


//Game variables
int cloudY = rand() % 60 +60;
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
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int xMin = 263;
int xMax = 393;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int yMin = 264;
int yMax = 395;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM
int zMin = 267;
int zMax = 397;
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMM

void loop(void) {
  playMusic();
  if(isGameOver) {
    gameOver(score);
    if (gameOverSequence >= 0)
    gameOverSequence -=1;
    delay(100);
    if (gameOverSequence == -1 && digitalRead(BUTTON) == HIGH) {
      isGameOver = false;
      isObstaclePresent = false;
      obstacleX = 240;
      score = 0;
      tft.fillRect(0,0, 240, 245, BLUE);
      gameOverSequence = 5;

    } 

  } else {

  

  collisionCheck();

  if (digitalRead(BUTTON) == HIGH) {
    jump();
  }
  if (digitalRead(BUTTON) == LOW){
      if (momentum < 0) {
        goingDown = true;
        momentum += 20;
      }
      else goingDown = false;
      playerY = 210 + momentum;
      isPressed = false;
  } 

 

  if (!isCloudPresent && rand() %15 == 1) {
    drawCloud();
  }
  if (isCloudPresent) {
    cloudX-=35;
    drawCloud();
  }

  displayScore();
  for(uint8_t frame=0; frame<2; frame++) {
    drawCharacter(frame);
  }

   if ( !isObstaclePresent && rand() % 10 == 1) {
    sendObstacle();
  }
  if (isObstaclePresent) {
    obstacleX -= 30;
    sendObstacle();
  }

  if (!isYarnPresent && rand() % 2 == 1) {
    sendYarn();
  }
  if (isYarnPresent) {
    if (!yarnDeflect) {
      yarnX-= 15;
      yarnY+=15;
    }
    else yarnY-=15;
    sendYarn();
  }

  checkAccel();
  
  }
}

unsigned long titleScreen() {
  unsigned long start = micros();
  tft.fillScreen(CYAN);
  tft.setCursor(60, 10);
  tft.setTextColor(WHITE);  tft.setTextSize(4); 
  tft.println("QUEST");
  tft.setCursor(85, 50);
  tft.println("FOR");
  tft.setTextSize(3.75);
  tft.setCursor(10, 90);
  tft.println("MEOWTROPOLIS");
}

unsigned long drawCharacter(uint8_t frame) {
  switch(frame) 
  {
    case 0: tft.fillRect(50, playerY, 25, 35, WHITE);
    break;
    case 1: tft.fillRect(50, playerY, 25, 35, RED);
    break;
    default: tft.fillRect(50, playerY, 25, 35, GREEN);
  }
  
  if (playerY < 210) {
   // Serial.println(playerY);
    tft.fillRect(50, playerY+35, 25, 210-(playerY), BLUE);
  }
  if (goingDown) {
    tft.fillRect(50, playerY-35, 25, 35, BLUE);
  }
  delay(40);
}

unsigned long drawCloud() {
  if (!isCloudPresent) {
      cloudY = rand() % 60 +60;
      int diff = rand() %2;
      cloud2Y = (diff) ? cloudY + 20 : cloudY - 20;
      if (cloud2Y >= 120) cloud2Y = 120;
      isCloudPresent = true;
  }

  tft.fillCircle(cloudX,cloudY,10, WHITE);
  tft.fillCircle(cloudX+10,cloudY,10,WHITE);
  tft.fillCircle(cloudX+20,cloudY,10,WHITE);
  tft.fillCircle(cloudX+8, cloudY-10, 10, WHITE);
  tft.fillCircle(cloudX+18, cloudY-10, 10, WHITE);
  tft.fillRect(cloudX+30, cloudY-20, 35, 40, BLUE);

  tft.fillCircle(cloudX+70,cloud2Y,10, WHITE);
  tft.fillCircle(cloudX+80,cloud2Y,10,WHITE);
  tft.fillCircle(cloudX+90,cloud2Y,10,WHITE);
  tft.fillCircle(cloudX+78, cloud2Y-10, 10, WHITE);
  tft.fillCircle(cloudX+88, cloud2Y-10, 10, WHITE);
  tft.fillRect(cloudX+100, cloud2Y-20, 35, 40, BLUE);

  if (cloudX < -100) {
    cloudX = 240;
    isCloudPresent = 0;
  }
}

unsigned long sendObstacle() {
  if (!isObstaclePresent) {
    obstacleW = rand() % 25 +10;
    obstacleH = rand() % 30 +10;
    isObstaclePresent = true;
  }
  
  tft.fillRect(obstacleX, 245-obstacleH, obstacleW, obstacleH, BLACK);
  tft.fillRect(obstacleX+obstacleW, 245-obstacleH, obstacleW+20, obstacleH, BLUE);

  if (obstacleX < -50) {
    obstacleX = 240;
    isObstaclePresent = false;
  }
}

unsigned long sendYarn() {
  if (!isYarnPresent)
  isYarnPresent = true;

  if (yarnDeflect) {
    tft.fillCircle(yarnX, yarnY, 10, YELLOW);
    tft.fillCircle(yarnX, yarnY+20, 10, BLUE);
  }else {
    tft.fillCircle(yarnX,yarnY, 10, YELLOW);
    tft.fillCircle(yarnX+15, yarnY-15, 10, BLUE);
  }
  if (yarnY < -20) {
    yarnDeflect = false;
    isYarnPresent = false;
    yarnX = 300;
    yarnY = 0;
  }

  

}

unsigned long displayScore() {
  tft.fillRect(0,5, 240, 30, BLUE);
  score+= 10;
  tft.setCursor(5, 5);
  tft.setTextSize(2);
  tft.println(score);

}

unsigned long jump() {
  if ( momentum > -60 && !isPressed) {
    momentum -= 20;
  }
  else if (momentum < 0){
    isPressed = true;
    goingDown = true;
    momentum += 20;
  }
  playerY = 210+momentum;
}

unsigned long collisionCheck() {
  if ((obstacleX >= 50 && obstacleX <= 75) || (obstacleX+obstacleW >= 50 && obstacleX+obstacleW <=75)) {
    if(245-obstacleH >= playerY && 245-obstacleH <= playerY+35) {
      delay(1000);
      isGameOver = true;
    }
    
  }
  else if (yarnX <= 75 && yarnY >= playerY) {
    delay(1000);
    isGameOver = true;
  }
}

unsigned long checkAccel() {

  if (quakeSequence > -1) {
    yarnDeflect = true;
    switch(quakeSequence) {
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
    quakeSequence --;
  }
  else {

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
  if (accelerationMagnitude > ACCELEROMETER_THRESHOLD) {
    quakeSequence = 3;
    }
}
}



unsigned long gameOver(int s) {
  switch(gameOverSequence) {
    case 5:
    tft.fillRect(50, playerY-20, 25, 35, WHITE);
    tft.fillRect(50, playerY-20+35, 25, 20, BLUE);
    break;
    case 4: 
    tft.fillRect(50, playerY+10, 25, 35, WHITE);
    tft.fillRect(50, playerY-20, 25, 30, BLUE);
    sendObstacle();
    break;
    case 3:
    tft.fillRect(50, playerY+40, 25, 35, WHITE);
    tft.fillRect(50, playerY+10, 25, 245-(playerY+10), BLUE);
    tft.fillRect(50, 245, 25, (playerY+40)-210, GREEN);
    sendObstacle();
    break;
    case 2:
    tft.fillRect(50, playerY+70, 25, 35, WHITE);
    tft.fillRect(50, playerY+40, 25, 245-(playerY+10), BLUE);
    tft.fillRect(50, 245, 25, (playerY+70)-210, GREEN);

    break;
    case 1: 
    tft.fillRect(50, playerY+90, 25, 35, WHITE);
    tft.fillRect(50, playerY+70, 25, 245-(playerY+10), BLUE);
    tft.fillRect(50, 245, 25, (playerY+90)-210, GREEN);
    break;
    case 0:
    tft.setCursor(60, 100);
    tft.println("GAME OVER");
    tft.setCursor(60, 120);
    tft.println("SCORE:");
    tft.setCursor(60,140);
    tft.println(s);
    delay(1000);
    tft.setCursor(60,160);
    tft.println("PRESS BUTTON");
    tft.setCursor(60,180);
    tft.println("TO RESTART");
    break;
  }

}

unsigned long playMusic() {
  Tone tone1;
  tone1.begin(15);
  tone1.play(NOTE_C3);
}

