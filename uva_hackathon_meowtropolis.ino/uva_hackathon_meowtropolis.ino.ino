#include <TFT.h> // Include the library for your specific LCD display

// Define the pins for the LCD
#define LCD_RS 22
#define LCD_D0 24
#define LCD_D1 25
#define LCD_D2 26
#define LCD_D3 27
#define LCD_D4 28
#define LCD_D5 29
#define LCD_D6 30
#define LCD_D7 31 

TFT lcd(0, LCD_RS, 0); // CS, RS, RST
void setup() {
  // Set up the LCD
  lcd.begin();
  // Print "Hello, World!" on the LCD
  lcd.setCursor(0, 0); // Set cursor to position (0, 0)
  lcd.println("Hello, World!");
}

void loop() {
  // No need for looping in this example
}


