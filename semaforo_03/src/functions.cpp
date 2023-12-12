#include "functions.h"

// ESP32Time rtc;
ESP32Time esp32_rtc; // offset in seconds GMT+3

// Objeto referente ao RTC
RTC_DS1307 rtc;
String hours_str;

// Cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wm;

PCF8574 canal1(0x27, SDA, SCL);
PCF8574 canal2(0x23, SDA, SCL);
PCF8574 canal3(0x21, SDA, SCL);
PCF8574 canal4(0x20, SDA, SCL);

LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 2);

void initOutputs()
{
  for (int i = 0; i < 6; i++)
  {
    canal1.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal2.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal3.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal4.pinMode(i, OUTPUT, 0);
  }
  canal1.begin();
  canal2.begin();
  canal3.begin();
  canal4.begin();
}

void initDisplay()
{
  lcd.init();
  lcd.clear();
  lcd.backlight();
  // lcd.noBacklight();
}

void initInputs()
{
  pinMode(BT_UP, INPUT_PULLUP);
  pinMode(BT_DOWN, INPUT_PULLUP);
  pinMode(BT_LEFT, INPUT_PULLUP);
  pinMode(BT_RIGHT, INPUT_PULLUP);
  pinMode(BT_CENTER, INPUT_PULLUP);
  pinMode(BT_PEDE, INPUT_PULLUP);
}

// Menus
Menu main_menu("Main menu", BT_UP, BT_DOWN, BT_RIGHT, BT_LEFT, BT_CENTER, BT_PEDE);

void mainScreen(void *p)
{
  for (;;)
  {
    lcd.setCursor(0, 0);
    lcd.printf("millis: %lu", millis() / 1000);
    delay(1000);
  }
}
