#ifndef MANAGER_H
#define MANAGER_H
#include <Arduino.h>
#include <vector>
#include "menu.h"
#define IDLE_MENU_LIMIT 15 // Limite de tempo para o menu ficar ocioso (sem ação do usuário) em segundos

class  Manager
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
  void showMenu();
};

#endif