#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

RTC_DS1307 rtc;

#define BUTTON_PIN 3
bool buttonPressed = false;

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 initialization failed!"));
    for (;;);
  }
  display.clearDisplay();

  dht.begin();

  if (!rtc.begin()) {
    Serial.println(F("RTC not found!"));
    for (;;);
  }
  if (!rtc.isrunning()) {
    Serial.println(F("RTC is NOT running, setting the time!"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 5);
  display.println(F("Clock"));
  display.display();
  delay(2000);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    buttonPressed = true;
  } else {
    buttonPressed = false;
  }

  display.clearDisplay();

  if (!buttonPressed) {
    DateTime now = rtc.now();

    display.setTextSize(2);
    String timeString = "";
    if (now.hour() < 10) timeString += '0';
    timeString += String(now.hour());
    timeString += ':';
    if (now.minute() < 10) timeString += '0';
    timeString += String(now.minute());
    timeString += ':';
    if (now.second() < 10) timeString += '0';
    timeString += String(now.second());

    display.setCursor(15, 0);
    display.print(timeString);

    display.setTextSize(1);
    display.setCursor(0, 13);
    display.print("---------------------");

    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print(F("Date: "));
    if (now.day() < 10) display.print('0');
    display.print(now.day());
    display.print('/');
    if (now.month() < 10) display.print('0');
    display.print(now.month());
    display.print('/');
    display.print(now.year());
  } else {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      display.setCursor(0, 0);
      display.println(F("Sensor Error!"));
    } else {
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print(F("T:"));
      display.print(temperature);
      display.print((char)247);
      display.print(F("C"));

      display.setCursor(0, 16);
      display.print(F("H:"));
      display.print(humidity);
      display.print(F("%"));
    }
  }
  display.display();

  delay(500);
}
