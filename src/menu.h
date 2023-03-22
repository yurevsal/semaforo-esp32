#ifndef MENU_H
#define MENU_H
#include <Arduino.h>
#include <vector>

class Menu
{
  String title;
  String current_title;
  int current_option_idx;
  std::vector<Menu *> sub_menus;
  std::vector<String> options;
  int goback; // Variavel que indica se este menu sera usado para retornar ao menu pai
  int minValue;
  int maxValue;
public:
  int* var;
  int myPosition;
  void (*_callback)(int); // Callback para salvar configurações ou realizar outras tarefas
  
  Menu(String t, int _goback = 0, int* _var = NULL, int _min=0, int _max=100, std::vector<String> _options={});
  void setOptionIdx(int n);
  int getOptionIdx();
  String getTitle();
  int subMenusLenght();
  String getSubMenuTitle(int index);
  int pushMenu(Menu *menu, void (*callback)(int) = nullptr);
  void popMenu();
  Menu *getSubMenu(int idx);
  int isGoBackOption();
  void replaceTitle(String s1, String s2);
  //Incrementa a variavel que o menu filho mantem controle
  //Altera o titulo do menu filho com a nova variavel
  void incrementVar(int idx, int num);
};

#endif
