#ifndef MANAGER_H
#define MANAGER_H
#include <Arduino.h>
#include <vector>
#include <queue>
#include "menu.h"
#define IDLE_MENU_LIMIT 15 // Limite de tempo para o menu ficar ocioso (sem ação do usuário) em segundos
// Eventos para fila de eventos
#define IDLE_EVENT      0x00
#define RTC_EVENT       0x10
#define KEYBOARD_EVENT  0x20
#define INPUT1_EVENT    0x30

class event
{
  public:
  int e;
  int value;
}

class Manager
{
private:
  int first_option;
  int currentOption;
  int vertical_idx;
  int horizontal_idx;
  int next_option = 0;
  std::vector<Menu *> sub_menus;
  std::vector<String> options;
  std::vector<int> parentOption;
  std::queue<event> events;
  unsigned long menuIdleCounterTime;

public:
  bool idle;
  bool refreshScreen;
  // int tempos[8][3];
  Manager();
  Menu *current_menu();
  int pushMenu(Menu *menu);
  void popMenu();
  void nextOption();
  void goBackOption();
  void nextSubMenu();
  void increaseOption();
  void decreaseOption();
  void showMenu(void *parameter);
  void switchPolling(void *parameter);
  void ledsLoop(void *parameter);
  void RTCLoop(void *parameter);
  void wifiSetup(void *parameter)
};

#endif