#include <Arduino.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Налаштування Wi-Fi та сервера
const char* ssid = "Xiaomi 17T";
const char* password = "12345678";
String serverName = "http://DENfan.pythonanywhere.com/update";

// Датчик DHT22 на D6
#define DHTPIN D6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Дисплей 1 (Температура, 128x32): SCL = D7, SDA = D5
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C displayTemp(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, U8X8_PIN_NONE);

// Дисплей 2 (Вологість, 128x32): SCL = D2, SDA = D1
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C displayHum(U8G2_R0, /* clock=*/ D2, /* data=*/ D1, U8X8_PIN_NONE);

// Функція виводу даних на обидва екрани
void updateDisplays(float t, float h) {
  char tempStr[16];
  char humStr[16];

  if (isnan(t)) {
    snprintf(tempStr, sizeof(tempStr), "T: -- C");
  } else {
    snprintf(tempStr, sizeof(tempStr), "T: %.1f C", t);
  }

  if (isnan(h)) {
    snprintf(humStr, sizeof(humStr), "H: -- %%");
  } else {
    snprintf(humStr, sizeof(humStr), "H: %.1f %%", h);
  }

  // Екран 1: Температура (D5/D7)
  displayTemp.clearBuffer();
  displayTemp.setFont(u8g2_font_logisoso16_tf);
  displayTemp.drawStr(0, 26, tempStr);
  displayTemp.sendBuffer();

  // Екран 2: Вологість (D1/D2)
  displayHum.clearBuffer();
  displayHum.setFont(u8g2_font_logisoso16_tf);
  displayHum.drawStr(0, 26, humStr);
  displayHum.sendBuffer();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Ініціалізація екранів
  displayTemp.begin();
  displayHum.begin();

  // Стартова індикація на екранах
  displayTemp.clearBuffer();
  displayTemp.setFont(u8g2_font_6x10_tf);
  displayTemp.drawStr(0, 18, "Connecting WiFi...");
  displayTemp.sendBuffer();

  displayHum.clearBuffer();
  displayHum.setFont(u8g2_font_6x10_tf);
  displayHum.drawStr(0, 18, "Reading DHT22...");
  displayHum.sendBuffer();

  delay(2000); // Даємо датчику час на зчитування

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Оновлюємо покази на екранах
  updateDisplays(t, h);

  // Підключення до Wi-Fi
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  // Відправка HTTP GET запиту на PythonAnywhere
  if (WiFi.status() == WL_CONNECTED && !isnan(t) && !isnan(h)) {
    WiFiClient client;
    HTTPClient http;

    String serverPath = serverName + "?t=" + String(t, 1) + "&h=" + String(h, 1);
    Serial.print("\nSending request to: ");
    Serial.println(serverPath);

    http.begin(client, serverPath);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("\nWiFi Connection failed or Sensor Error!");
  }

  WiFi.disconnect();

  Serial.println("Going to deep sleep for 30s...");
  ESP.deepSleep(30e6); // 30 секунд глибокого сну
}

void loop() {
  // При Deep Sleep вся логіка виконується в setup()
}