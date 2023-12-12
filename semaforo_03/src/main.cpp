#include "defines.h"
#include "functions.h"

extern Menu main_menu;

void setup(){
  Serial.begin(115200);
  initOutputs();
  initDisplay();
  main_menu.setMenuInterface(mainScreen);
  main_menu.show();

}
void loop(){
  Serial.println("loop");
  delay(1000);
}