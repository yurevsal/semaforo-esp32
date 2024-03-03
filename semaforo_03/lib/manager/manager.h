#ifndef MANAGER_H
#define MANAGER_H
#include <Arduino.h>
#include <stack>
#include <menu.h>

#define BT_UP_PIN 27
#define BT_DOWN_PIN 32
#define BT_LEFT_PIN 26
#define BT_RIGHT_PIN 33
#define BT_CENTER_PIN 25
#define BT_PEDE_PIN 35

using namespace std;

class Context
{
  public:
  int8_t next_menu;
  int8_t go_back;
  volatile int v_counter;//Contador de teclas verticais(up, down)
  volatile int h_counter;// Contador de teclas horizontais(left,right)
  volatile int enter_counter;
  Menu *current_menu;
};

class Manager
{
  stack<Menu*> menu_stack;
  void initBtns();
  public:
  Context context;
  uint8_t bt_up_status;
  uint8_t bt_down_status;
  uint8_t bt_left_status;
  uint8_t bt_right_status;
  uint8_t bt_center_status;
  uint8_t bt_pede_status;
  uint8_t confirm_counter;
  void begin(Menu *m);
  void showMenu();
  void pushMenu(Menu *m);
};
extern Manager* global_manager;
#endif