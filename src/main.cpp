#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "Xiaomi 17T";
const char* password = "12345678";
String serverName = "http://DENfan.pythonanywhere.com/update";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN D6
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
  
  delay(4000); 

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Env Monitor");

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("T: ");
  if (isnan(t)) {
    display.print("--");
  } else {
    display.print(t, 1);
  }
  display.print(" C");

  display.setCursor(0, 45);
  display.print("H: ");
  if (isnan(h)) {
    display.print("--");
  } else {
    display.print(h, 1);
  }
  display.print(" %");

  display.display();

  Serial.println();
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED && !isnan(t) && !isnan(h)) {
    WiFiClient client;
    HTTPClient http;
    
    String serverPath = serverName + "?t=" + String(t) + "&h=" + String(h);
    
    http.begin(client, serverPath);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("\nHTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("\nError code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  
  WiFi.disconnect(); 
  
  Serial.println("Going to deep sleep...");
  ESP.deepSleep(30e6); 
}

void loop() {
}