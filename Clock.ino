#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

#define DHTPIN 2
#define DHTTYPE DHT22

#define BUTTON_PIN 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

RTC_DS1307 rtc;

bool buttonPressed = false;
bool lastButtonState = HIGH;
unsigned long buttonPressTime = 0;
const unsigned long displayDuration = 4000;

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

  loading();
  delay(300);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(12, 15);
  display.println(F("PG Gen. Vl. Zaimov"));
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.print("----------");
  display.setTextSize(1);
  display.setCursor(50, 40);
  display.println(F("Sopot"));
  display.display();
  delay(3000);

  drawRect();
  fillRect();

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(40, 4);
  display.println(F("Made by:"));
  display.setTextSize(2);
  display.setCursor(0, 13);
  display.print("----------");
  display.setCursor(14, 25);
  display.println(F("Gabriela"));
  display.setCursor(14, 45);
  display.println(F("Vasileva"));
  display.display();
  delay(3000);
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    buttonPressed = true;
    buttonPressTime = millis();
  }
  lastButtonState = currentButtonState;

  display.clearDisplay();

  if (buttonPressed) {
    if (millis() - buttonPressTime > displayDuration) {
      buttonPressed = false;
    } else {
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();

      if (isnan(temperature) || isnan(humidity)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        display.setCursor(0, 0);
        display.println(F("Sensor Error!"));
      } else {
        display.setTextSize(1);
        display.setCursor(0, 10);
        display.print(F("Temp: "));
        display.setTextSize(2);
        display.print(temperature, 2);
        display.print((char)247);
        display.print(F("C"));

        display.setCursor(0, 25);
        display.print("----------");
        display.setTextSize(1);
        display.setCursor(0, 40);
        display.print(F("Hum: "));
        display.setTextSize(2);
        display.print(humidity, 2);
        display.print(F("%"));
      }
    }
  }

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
    display.setTextSize(2);
    display.setCursor(15, 10);
    display.print(timeString);
    display.setCursor(0, 25);
    display.print("----------");
    display.setCursor(0, 40);
    if (now.day() < 10) display.print('0');
    display.print(now.day());
    display.print('/');
    if (now.month() < 10) display.print('0');
    display.print(now.month());
    display.print('/');
    display.print(now.year());
  }
  display.display();
  delay(100);
}

void loading(void) {
  for (int i = 14; i <= 100; i++)
  { display.clearDisplay();
    display.drawRoundRect(12, 20, 104, 20, 10 , SSD1306_WHITE);
    display.fillRoundRect(14, 23, i, 14, 10, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(30, 50);
    display.print("Loading ");
    display.print(i);
    display.print("%");
    display.display();
  }
}

void drawRect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(500);
}

void fillRect(void) {
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
  delay(500);
}
