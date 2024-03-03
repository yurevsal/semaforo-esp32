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

class Menu{
    public:
    vector<Menu*> sub_menus;
    void (*_callback)(void*);
    String title;
    Menu(String title);
    void addSubMenu(Menu *m, void (*callback)(void*) = nullptr);
    void setMenuInterface(void (*callback)(void*) = nullptr);
    void show(void* context = nullptr);

};
#endif