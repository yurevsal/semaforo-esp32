#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include <vector>

using namespace std;

namespace Keyboard
{
  enum e
  {
    PRESSED,
    RELEASED
  };
}

class KeyPin{
    public:
    int gpio;
    int state;
};

class Menu{
    vector<Menu*> sub_menus;
    KeyPin key_u;
    KeyPin key_d;
    KeyPin key_r;
    KeyPin key_l;
    KeyPin key_c;
    KeyPin key_p;
    void (*_callback)(void*);

    public:
    Menu(String title, int key_up,int key_down,int key_right,int key_left,int key_center, int key_pede);
    void addSubMenu(Menu *m, void (*callback)(void*) = nullptr);
    void setMenuInterface(void (*callback)(void*) = nullptr);
    void show(void* context = nullptr);
    void switchCheck();

};
#endif