#include "menu.h"

Menu::Menu(String title)
{
  this->title = title;
}

void Menu::setMenuInterface(void (*callback)(void*))
{
  _callback = callback;
}

void Menu::addSubMenu(Menu *m, void (*callback)(void*))
{
  m->setMenuInterface(callback);
  sub_menus.push_back(m);
}

 void Menu::show(void* context){
  _callback(context);
 }



