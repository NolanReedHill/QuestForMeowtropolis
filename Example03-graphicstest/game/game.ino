// IMPORTANT: ELEGOO_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Elegoo_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <SPI.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xf483
#define GRAY    0xa4d1
#define PINK    0xe558
#define BROWN   0x8b06
#define LBROWN  0xc52f



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

  // Serial.print(F("Screen fill              "));
  // Serial.println(testFillScreen());
  // delay(500);

  // Serial.print(F("Text                     "));
  // Serial.println(testText());
  // delay(3000);

  // Serial.print(F("Lines                    "));
  // Serial.println(testLines(CYAN));
  // delay(500);

  // Serial.print(F("Horiz/Vert Lines         "));
  // Serial.println(testFastLines(RED, BLUE));
  // delay(500);

  // Serial.print(F("Rectangles (outline)     "));
  // Serial.println(testRects(GREEN));
  // delay(500);

  // Serial.print(F("Rectangles (filled)      "));
  // Serial.println(testFilledRects(YELLOW, MAGENTA));
  // delay(500);

  // Serial.print(F("Circles (filled)         "));
  // Serial.println(testFilledCircles(10, MAGENTA));

  // Serial.print(F("Circles (outline)        "));
  // Serial.println(testCircles(10, WHITE));
  // delay(500);

  // Serial.print(F("Triangles (outline)      "));
  // Serial.println(testTriangles());
  // delay(500);
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
int isCloudPresent = 0;

void loop(void) {

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

// drawing big cat for the main title screen
  tft.fillRect(60,120,30,10,ORANGE);
  tft.fillRect(160,130,30,10,ORANGE);
  tft.fillRect(60,130,10,10,ORANGE);
  tft.fillRect(70,130,10,10,BROWN);
  tft.fillRect(80,130,10,10,ORANGE);
  tft.fillRect(160,130,10,10,ORANGE);
  tft.fillRect(170,130,10,10,BROWN);
  tft.fillRect(170,140,20,10,BROWN);
  tft.fillRect(180,130,20,10,ORANGE);
  tft.fillRect(50,140,20,10,ORANGE);
  tft.fillRect(70,140,20,10,BROWN);
  tft.fillRect(90,140,10,10,ORANGE);
  tft.fillRect(150,140,20,10,ORANGE);
  tft.fillRect(190,140,10,10,ORANGE);
  tft.fillRect(50,150,10,10,ORANGE);
  tft.fillRect(60,150,30,10,BROWN);
  tft.fillRect(90,150,10,10,ORANGE);
  tft.fillRect(150,150,10,10,ORANGE);
  tft.fillRect(160,150,30,10,BROWN);
  tft.fillRect(190,150,20,10,ORANGE);
  tft.fillRect(30,160,30,10,GRAY);
  tft.fillRect(60,160,120,20,ORANGE);
  tft.fillRect(200,160,10,10,ORANGE);
  tft.fillRect(210,160,20,10,GRAY);
  tft.fillRect(200,170,20,10,GRAY);
  tft.fillRect(30,160,30,10,GRAY);
  tft.fillRect(50,170,10,10,GRAY);
  tft.fillRect(20,180,30,10,GRAY);
  tft.fillRect(50,180,30,10,ORANGE);
  tft.fillRect(50,190,150,50,ORANGE);
  tft.fillRect(80,180,20,20,WHITE);
  tft.fillRect(50,190,10,10,GRAY);
  tft.fillRect(90,190,10,10,BLACK);
  tft.fillRect(80,220,10,10,BLACK);
  tft.fillRect(150,180,20,20,WHITE);
  tft.fillRect(150,190,10,10,BLACK);
  tft.fillRect(200,190,30,10,GRAY);
  tft.fillRect(30,200,20,10,GRAY);
  tft.fillRect(120,200,10,10,LBROWN);
  tft.fillRect(130,200,10,10,PINK);
  tft.fillRect(120,210,10,10,PINK);
  tft.fillRect(80,220,10,10,BLACK);
  tft.fillRect(90,230,20,10,BLACK);
  tft.fillRect(120,220,10,10,BLACK);
  tft.fillRect(150,220,10,10,BLACK);
  tft.fillRect(60,230,120,10,ORANGE);
  tft.fillRect(50,230,10,10,GRAY);
  tft.fillRect(90,230,20,10,BLACK);
  tft.fillRect(130,230,20,10,BLACK);
  tft.fillRect(150,230,10,10,BLACK);
  tft.fillRect(200,190,30,10,GRAY);
  tft.fillRect(200,200,10,20,ORANGE);
  tft.fillRect(210,210,20,10,GRAY);
  tft.fillRect(200,220,10,10,GRAY);
  tft.fillRect(60,240,20,10,GRAY);
  tft.fillRect(80,240,90,10,ORANGE);
  tft.fillRect(180,240,10,10,GRAY);
  tft.fillRect(80,250,90,10,GRAY);
  tft.fillRect(160,120,30,10,ORANGE);
  tft.fillRect(200,180,10,10,ORANGE);
  tft.fillRect(180,160,10,30,ORANGE);
  tft.fillRect(170,180,20,10,ORANGE);
  tft.fillRect(100,180,50,10,ORANGE);
  tft.fillRect(170,240,10,10,GRAY);
  tft.fillRect(190, 160, 10, 30, ORANGE);
  tft.fillRect(110,230,10,10,BLACK);


}

unsigned long drawCharacter(uint8_t frame) {
  switch(frame) 
  {
    case 0:
    tft.fillRect(58, 219, 12, 4, ORANGE);
    tft.fillRect(56, 218, 2, 6, ORANGE);
    tft.fillRect(67,215,3,3,ORANGE);
    tft.fillRect(68,216,3,5,ORANGE);
    tft.fillRect(57,210,1,1,ORANGE);
    tft.fillRect(56,211,3,1,ORANGE);
    tft.fillRect(55,212,5,1,ORANGE);
    tft.fillRect(55,213,2,1,ORANGE);
    tft.fillRect(57,213,1,1,BROWN);
    tft.fillRect(58,213,2,1,ORANGE);
    tft.fillRect(55,214,1,1,ORANGE);
    tft.fillRect(56,214,3,1,BROWN);
    tft.fillRect(59,214,1,1,ORANGE);
    tft.fillRect(55,215,14,4,ORANGE);
    tft.fillRect(70,216,1,1,ORANGE);
    tft.fillRect(67,211,2,1,ORANGE);
    tft.fillRect(66,212,2,1,ORANGE);
    tft.fillRect(65,213,5,2,ORANGE);
    tft.fillRect(67,215,3,2,BROWN);
    tft.fillRect(68,212,1,1,BROWN);
    tft.fillRect(69,211,1,2,ORANGE);
    tft.fillRect(55,215,14,5,ORANGE);
    tft.fillRect(70,216,1,13,ORANGE);
    tft.fillRect(71,217,1,5,ORANGE);
    tft.fillRect(72,217,1,1,GRAY);
    tft.fillRect(72,219,2,1,GRAY);
    tft.fillRect(60,216,2,2,WHITE);
    tft.fillRect(61,217,1,1,BLACK);
    tft.fillRect(65,216,2,2,WHITE);
    tft.fillRect(66,217,1,1,BLACK);  
    tft.fillRect(62,219,1,1,PINK);
    tft.fillRect(61,220,1,1,BLACK);
    tft.fillRect(62,221,1,1,BLACK);
    tft.fillRect(63,220,1,1,BLACK);
    tft.fillRect(64,221,2,1,BLACK);
    tft.fillRect(66,220,1,1,BLACK);
    tft.fillRect(54,217,3,1,GRAY);
    tft.fillRect(54,219,2,1,GRAY);
    tft.fillRect(56,221,1,1,GRAY);
    tft.fillRect(57,222,1,1,GRAY);
    tft.fillRect(58,223,12,1,GRAY);
    tft.fillRect(70,222,1,1,GRAY);
    tft.fillRect(71,221,1,1,GRAY);
    tft.fillRect(70,223,2,1,ORANGE);
    tft.fillRect(54,223,4,1,ORANGE);
    tft.fillRect(50,224,2,10,ORANGE);
    tft.fillRect(52,232,2,2,ORANGE);
    tft.fillRect(54,233,1,2,ORANGE);
    tft.fillRect(55,233,2,2,ORANGE);
    tft.fillRect(54,222,3,7,ORANGE);
    tft.fillRect(53,229,2,2,ORANGE);
    tft.fillRect(54,229,1,1,GRAY);
    tft.fillRect(53,230,1,1,GRAY);
    tft.fillRect(57,224,13,11,ORANGE);
    tft.fillRect(57,235,5,6,ORANGE);
    tft.fillRect(57,241,4,1,ORANGE);
    tft.fillRect(58,242,3,1,ORANGE);
    tft.fillRect(57,243,3,2,ORANGE);
    tft.fillRect(58,243,1,1,GRAY);
    tft.fillRect(57,244,1,1,GRAY);
    tft.fillRect(59,244,1,1,GRAY);
    tft.fillRect(63,235,5,6,ORANGE);
    tft.fillRect(64,241,4,2,ORANGE);
    tft.fillRect(65,243,4,2,ORANGE);
    tft.fillRect(67,243,1,1,GRAY);
    tft.fillRect(66,244,1,1,GRAY);
    tft.fillRect(68,244,1,1,GRAY);
    tft.fillRect(70,224,2,5,ORANGE);
    tft.fillRect(72,226,1,1,ORANGE);
    tft.fillRect(72,227,2,3,ORANGE);
    tft.fillRect(73,228,2,3,ORANGE);
    tft.fillRect(73,229,1,1,GRAY);
    tft.fillRect(74,230,1,1,GRAY);

    










    break;
    case 1: //tft.fillRect(50, 210, 25, 35, RED);









    break;
    default: tft.fillRect(70, 170, 25, 35, WHITE);
  }
  delay(60);
}

unsigned long drawCloud() {
  if (!isCloudPresent) {
      cloudY = rand() % 60 +60;
      int diff = rand() %2;
      cloud2Y = (diff) ? cloudY + 20 : cloudY - 20;
      if (cloud2Y >= 120) cloud2Y = 120;
      isCloudPresent = 1;
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

unsigned long displayScore() {
  tft.fillRect(0,5, 240, 30, BLUE);
  score+= 10;
  tft.setCursor(5, 5);
  tft.setTextSize(2);
  tft.println(score);

}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(0, 0, i));
  }

  return micros() - start;
}

