#ifndef ESP32_UTILS_H
#define ESP32_UTILS_H
#include <vector>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <driver/adc.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <HTTPClient.h>
#include <esp_task_wdt.h>
#include "SPIFFS.h"
#include "FS.h"
#define WIFI_SETTINGS "/networks.txt"

//lib_deps =  arduino-libraries/ArduinoHttpClient@^0.4.0
void enableWDT(int _time=60);
bool enableWifi(String ssid, String password, int timeout);
void disableRadio();
String readFile(String path);
bool writeFile(String path, String message);
std::vector<String> getNetworksCredentials(String path);
int autoConnectWiFi();
#endif