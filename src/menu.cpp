#include "menu.h"

  Menu::Menu(String t, int _goback , int* _var , int _min, int _max, std::vector<String> _options)
  {
    title = t;
    current_option_idx = 0;
    goback = _goback;
    var = _var;
    options = _options;
    minValue = _min;
    maxValue = _max;
  }

  void Menu::setOptionIdx(int n)
  {
    current_option_idx = n;
  }
  int Menu::getOptionIdx()
  {
    return current_option_idx;
  }
  String Menu::getTitle()
  {
    current_title = title;
    if(var != NULL)
    {
      if(options.empty())
        current_title.replace("{0}", String(*var));
      else
        current_title.replace("{0}", options[*var]);
    }
    return current_title;
  }
  int Menu::subMenusLenght()
  {
    return sub_menus.size();
  }
  String Menu::getSubMenuTitle(int index)
  {
    if (sub_menus.empty())
      return "";
    else
      return sub_menus[index]->getTitle();
  }
  int Menu::pushMenu(Menu *menu, void (*callback)(int))
  {
    menu->_callback = callback;
    sub_menus.push_back(menu);
    return sub_menus.size() - 1;
  }
  void Menu::popMenu()
  {
    sub_menus.pop_back();
  }
  Menu *Menu::getSubMenu(int idx)
  {
    return sub_menus[idx];
  }
  int Menu::isGoBackOption()
  {
    return goback;
  }
  void Menu::replaceTitle(String s1, String s2)
  {
    title.replace(s1, s2);
  }
  //Incrementa a variavel que o menu filho mantem controle
  //Altera o titulo do menu filho com a nova variavel
  void Menu::incrementVar(int idx, int num)
  {
    //Checa se existe uma variavel atribuida aquele menu
    if(sub_menus[idx]->var != NULL)
    {
      //Incrementa a variavel
    *sub_menus[idx]->var += num;
    //Limita a variavel a valores maximo e minimo
    if(*sub_menus[idx]->var < sub_menus[idx]->minValue) 
      *sub_menus[idx]->var = sub_menus[idx]->maxValue;

    if(*sub_menus[idx]->var > sub_menus[idx]->maxValue) 
      *sub_menus[idx]->var = sub_menus[idx]->minValue;
    }
  }