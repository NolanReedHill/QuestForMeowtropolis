#define RT0 10000  
#define B 3977      
#define VCC 5   
#define R 10000 // R=10KΩ

float RT, VR, ln, TX,  T0, VRT;

int spriteX = 50;
int spriteY = 50;

void setup() {
  Serial.begin(9600);
  
  T0 = 25 + 273.15; 
}

void loop() {
  // Read thermistor temperature
  VRT = analogRead(A7);              
  VRT  = (5.00 / 1023.00) * VRT;     
  VR = VCC - VRT;
  RT = VRT / (VR / R);               

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0)));  

  TX =  TX - 273.15;                 

  if (TX > 25) {
    Serial.println("Temperature too high - drawing circle");
  }

  delay(1000); 
}
