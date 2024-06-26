#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library for Elegoo TFT LCD

#define YP A3 // Must be an analog pin, use "An" notation!
#define XM A2 // Must be an analog pin, use "An" notation!
#define YM 9  // Can be a digital pin
#define XP 8  // Can be a digital pin
#define THERM A7 //Thermister

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// For 2.8" TFT LCD Shield
#define TS_MINX 0
#define TS_MINY 0
#define TS_MAXX 240
#define TS_MAXY 320

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define RT0 10000 // Ω
#define B 3977    //  K
//--------------------------------------

#define VCC 5   // Supply  voltage
#define R 10000 // R=10KΩ

// Variables
float RT, VR, ln, TX, T0, VRT;

void setup()
{
  Serial.begin(9600);

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  // Initialize LCD
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
  tft.setRotation(3); // Adjust rotation if needed

  // Clear the screen
  tft.fillScreen(BLACK);

  T0 = 25 + 273.15; // Temperature  T0 from datasheet, conversion from Celsius to kelvin
}

void loop()
{
  VRT = analogRead(THERM);         // Acquisition analog value of VRT
  VRT = (5.00 / 1023.00) * VRT; // Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R); // Resistance of RT

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); // Temperature from thermistor

  TX = TX - 273.15; // Conversion to Celsius

  // Draw a circle around the sprite when the thermistor is touched
  if (thermistorTouched())
  {
    tft.setCursor(150, 100);
    tft.drawCircle(100, 100, 5, WHITE);
    delay(10);                   // Adjust delay as needed
    tft.fillScreen(BLACK); // Clear the screen after drawing
  }

  // You can add more game logic here

  delay(5);
}

bool thermistorTouched()
{
  float temp = analogRead(THERM);
  tft.println(TX);
  return true;
}