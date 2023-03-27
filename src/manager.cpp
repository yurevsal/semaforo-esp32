#include "manager.h"

  Manager::Manager()
  {
    first_option = 0;
    currentOption = 0;
    refreshScreen = true;
    vertical_idx = 0;
    horizontal_idx = 0;
    idle = true;
  }
  Menu *Manager::current_menu()
  {
    return sub_menus.back();
  }
  int Manager::pushMenu(Menu *menu)
  {
    sub_menus.push_back(menu);
    return sub_menus.size() - 1;
  }
  void Manager::popMenu()
  {
    sub_menus.pop_back();
  }

  void Manager::nextOption()
  {
    menuIdleCounterTime = millis();
    if(idle)
    {
      idle = false;
      return;
    }
    // Avança na navegação vertical
    vertical_idx += 1;
    vertical_idx = (vertical_idx >= current_menu()->subMenusLenght() - 1) ? current_menu()->subMenusLenght() - 1 : vertical_idx;
    current_menu()->setOptionIdx(vertical_idx);
    refreshScreen = true;
  }
  void Manager::goBackOption()
  {
    menuIdleCounterTime = millis();
    if(idle)
    {
      idle = false;
      return;
    }
    // Recua na navegação vertical
    vertical_idx -= 1;
    vertical_idx = (vertical_idx <= 0) ? 0 : vertical_idx;
    current_menu()->setOptionIdx(vertical_idx);
    refreshScreen = true;
  }
  void Manager::nextSubMenu()
  {
    menuIdleCounterTime = millis();
    if(idle)
    {
      idle = false;
      return;
    }
    int idx = current_menu()->getOptionIdx();
    Menu *m = current_menu()->getSubMenu(idx);
    m->myPosition = idx;
    // Se o menu é do tipo "goBack" retorne ao menu pai
    // Removemdo o menu atual da pilha
    if (m->isGoBackOption() == 1)
    {
      Serial.println("Voltando");
      if(m->_callback)
      {
        Serial.println("Tem Callback");
        //Salvando as configurações -> void saveTimes(int fase)
        m->_callback(parentOption.back());
      }
      else 
        Serial.println("Não tem Callback");
      
      popMenu();
      vertical_idx = parentOption.back();

      parentOption.pop_back();
      refreshScreen = true;
      return;
    }
    // Insere o proximo menu na pilha

    if (m->subMenusLenght() > 0)
    {
      Serial.println("Tem submenus");
      // // Condição especial
      // if(m->getTitle().indexOf("Fase ") != -1)
      // {
      //   // Serial.printf("Quantidade de submenus: %d\t indice atual: %d\n",m->subMenusLenght(), idx);
      //   for(int i=0; i<m->subMenusLenght()-1;i++)
      //   {
      //     m->getSubMenu(i+1)->var = &tempos[idx-1][i];
      //     Serial.printf("Tempo[%d][%d] = %d\n", idx-1, i, tempos[idx-1][i]);
      //   }
      // }
      sub_menus.push_back(m);
      parentOption.push_back(vertical_idx);
      vertical_idx = 0;
      refreshScreen = true;
      return;
    }
    else
      Serial.println("Sem submenus");
  }
  void Manager::increaseOption()
  {
    menuIdleCounterTime = millis();
    if(idle)
    {
      idle = false;
      return;
    }
    current_menu()->incrementVar(vertical_idx, 1);
    refreshScreen = true;
  }
  void Manager::decreaseOption()
  {
    menuIdleCounterTime = millis();
    if(idle)
    {
      idle = false;
      return;
    }
    current_menu()->incrementVar(vertical_idx, -1);
    refreshScreen = true;
  }