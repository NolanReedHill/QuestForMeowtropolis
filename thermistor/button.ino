#include <Elegoo_GFX.h>    
#include <Elegoo_TFTLCD.h> 

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BUTTON_PIN A6

// players position:
int spriteX = SCREEN_WIDTH / 2;
int spriteY = SCREEN_HEIGHT - 20;
int jumpHeight = 40; 

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    jump();
  }
  delay(100); 
}

// this moves the sprite up
void jump() {
  tft.fillCircle(spriteX, spriteY, 5, ILI9341_BLACK); 
  spriteY -= jumpHeight;
  if (spriteY < 0) {
    spriteY = 0;
  }
  tft.fillCircle(spriteX, spriteY, 5, ILI9341_WHITE); 

  delay(200);

  // this makes sprite move down
  tft.fillCircle(spriteX, spriteY, 5, ILI9341_BLACK); 
  spriteY += jumpHeight;
  if (spriteY >= SCREEN_HEIGHT) {
    spriteY = SCREEN_HEIGHT - 20; 
  }
  tft.fillCircle(spriteX, spriteY, 5, ILI9341_WHITE); 
}
