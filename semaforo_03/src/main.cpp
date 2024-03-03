#include "defines.h"
#include "functions.h"

extern Menu main_menu;
extern WiFiManager wm;
Manager manager;


void setup(){
  Serial.begin(115200);
  initOutputs();
  initDisplay();
  initWiFiManager();
  mqttSetup();
  initMenus();
  main_menu.setMenuInterface(mainScreen);
  manager.begin(&main_menu);
  xTaskCreate(reconnect, "reconnect_client", 4048, NULL, 10, NULL);
}
void loop(){
  manager.showMenu();
  process();
  delay(100);
}