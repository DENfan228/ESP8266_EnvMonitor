#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED error"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Lviv Poly / Env Monitor");

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("T: ");
  display.print(t, 1);
  display.print(" C");

  display.setCursor(0, 45);
  display.print("H: ");
  display.print(h, 1);
  display.print(" %");

  display.display();
  delay(2000);
}