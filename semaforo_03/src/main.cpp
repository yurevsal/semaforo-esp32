#include "defines.h"
#include "functions.h"

extern Menu main_menu;
extern WiFiManager wm;

// class Context
// {
//   public:
//   Menu parent;
// };

void setup(){
  Serial.begin(115200);
  initOutputs();
  initDisplay();
  initWiFiManager();
  mqttSetup();
  // main_menu.setMenuInterface(mainScreen);
  // main_menu.show();
  xTaskCreate(reconnect, "reconnect_client", 4048, NULL, 10, NULL);

}
void loop(){
  process();
}