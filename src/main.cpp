#include <Arduino.h>
#include <Wire.h>

void setup() {
  // Ініціалізуємо I2C на пінах D2 (SDA) та D1 (SCL)
  Wire.begin(D2, D1);
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nI2C Scanner запрацював...");
}

void loop() {
  byte error, address;
  int nDevices = 0;
  
  Serial.println("Сканування шини I2C...");

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Знайдено пристрій на адресі: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    } 
    else if (error == 4) {
      Serial.print("Невідома помилка на адресі 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("Жодного I2C пристрою не знайдено! (Екран не підключений або згорів/немає живлення)\n");
  else
    Serial.println("Сканування закінчено.\n");

  delay(5000);
}