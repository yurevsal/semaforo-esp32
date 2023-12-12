#include "menu.h"

Menu::Menu(String title, int key_up, int key_down, int key_right, int key_left, int key_center, int key_pede)
{
  key_u.gpio = key_up;
  key_d.gpio = key_down;
  key_r.gpio = key_right;
  key_l.gpio = key_left;
  key_c.gpio = key_center;
  key_p.gpio = key_pede;

  key_u.state = Keyboard::RELEASED;
  key_d.state = Keyboard::RELEASED;
  key_r.state = Keyboard::RELEASED;
  key_l.state = Keyboard::RELEASED;
  key_c.state = Keyboard::RELEASED;
  key_p.state = Keyboard::RELEASED;
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

void Menu::switchCheck()
{
  pinMode(2, OUTPUT);
  digitalWrite(2,
               !digitalRead(key_u.gpio) ||
                   !digitalRead(key_d.gpio) ||
                   !digitalRead(key_l.gpio) ||
                   !digitalRead(key_r.gpio) ||
                   !digitalRead(key_c.gpio) ||
                   !digitalRead(key_p.gpio));
  if (!digitalRead(key_u.gpio))
  {
    vTaskDelay(250 / portTICK_PERIOD_MS);
    key_u.state = Keyboard::PRESSED;
  }
  else
  {
    if (key_u.state == Keyboard::PRESSED)
    {
      key_u.state = Keyboard::RELEASED;
    }
  }
  if (!digitalRead(key_d.gpio))
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    key_d.state = Keyboard::PRESSED;
  }
  else
  {
    if (key_d.state == Keyboard::PRESSED)
    {
      key_d.state = Keyboard::RELEASED;
    }
  }
  if (!digitalRead(key_l.gpio))
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    key_l.state = Keyboard::PRESSED;
  }
  else
  {
    if (key_l.state == Keyboard::PRESSED)
    {
      key_l.state = Keyboard::RELEASED;
    }
  }
  if (!digitalRead(key_r.gpio))
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    key_r.state = Keyboard::PRESSED;
  }
  else
  {
    if (key_r.state == Keyboard::PRESSED)
    {
      // Incrementa uma variavel que o menu controla
      key_r.state = Keyboard::RELEASED;
    }
  }
  if (!digitalRead(key_c.gpio))
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    key_c.state = Keyboard::PRESSED;
  }
  else
  {
    if (key_c.state == Keyboard::PRESSED)
    {
      key_c.state = Keyboard::RELEASED;
    }
  }
  if (!digitalRead(key_p.gpio))
  {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    key_p.state = Keyboard::PRESSED;
  }
  else
  {
    if (key_p.state == Keyboard::PRESSED)
    {
      key_p.state = Keyboard::RELEASED;
    }
  }
}

 void Menu::show(void* context){
  _callback(context);
 }



