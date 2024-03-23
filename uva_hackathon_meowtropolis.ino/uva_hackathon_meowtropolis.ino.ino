#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library for Elegoo TFT LCD

#define YP A3 // Must be an analog pin, use "An" notation!
#define XM A2 // Must be an analog pin, use "An" notation!
#define YM 9  // Can be a digital pin
#define XP 8  // Can be a digital pin

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

  // Initialize LCD
  tft.reset();
  uint16_t identifier = tft.readID();
  if (identifier == 0x9325)
  {
    Serial.println(F("Found ILI9325 LCD driver"));
  }
  else
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    return;
  }
  tft.begin(identifier);
  tft.setRotation(3); // Adjust rotation if needed

  // Clear the screen
  tft.fillScreen(ILI9341_BLACK);

  T0 = 25 + 273.15; // Temperature  T0 from datasheet, conversion from Celsius to kelvin
}

void loop()
{
  VRT = analogRead(A0);         // Acquisition analog value of VRT
  VRT = (5.00 / 1023.00) * VRT; // Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R); // Resistance of RT

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); // Temperature from thermistor

  TX = TX - 273.15; // Conversion to Celsius

  // Draw a circle around the sprite when the thermistor is touched
  if (thermistorTouched())
  {
    tft.drawCircle(spriteX, spriteY, 5, ILI9341_WHITE);
    delay(1000);                   // Adjust delay as needed
    tft.fillScreen(ILI9341_BLACK); // Clear the screen after drawing
  }

  // You can add more game logic here

  delay(500);
}

bool thermistorTouched()
{
  // Implement logic to detect if the thermistor is touched
  // This could involve checking the temperature and comparing it with a threshold
  // For simplicity, let's assume touching the thermistor always triggers the event
  return true;
}
