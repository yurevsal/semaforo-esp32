#include <Arduino.h>
#include <NTPClient.h> 
#include <esp_task_wdt.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <SPI.h>
#include <Wire.h>        
#include <PCF8574.h>
#include <RTClib.h>
#include <vector>
#include <Preferences.h>
#include <ESP32Time.h>
#include <LiquidCrystal_I2C.h>
#include <menu.h>

// I2C
#define LCD_ADDRESS         0x22

// GPIO
#define SCL 22
#define SDA 21
#define BT_UP 27
#define BT_DOWN 32
#define BT_LEFT 26
#define BT_RIGHT 33
#define BT_CENTER 25
#define BT_PEDE 35 // Botoeira dos pedestres