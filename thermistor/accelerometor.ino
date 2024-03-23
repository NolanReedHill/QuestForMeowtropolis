#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

#define ACCELEROMETER_THRESHOLD 8000 

void setup() {
  Serial.begin(9600);

  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  Serial.println("MPU6050 connection successful");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float accelerationMagnitude = sqrt(ax * ax + ay * ay + az * az);

  if (accelerationMagnitude > ACCELEROMETER_THRESHOLD) {
    Serial.println("Attack!");
    // Add your attack logic here, ex: attack triggering animation a heehee
  }

  delay(100); 
}
