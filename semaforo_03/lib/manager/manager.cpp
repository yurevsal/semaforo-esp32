#include "manager.h"

Manager *global_manager;

void Manager::begin(Menu *m)
{
  global_manager = this;
  menu_stack.push(m);
  context.h_counter = 10;
  context.v_counter = 10;
  initBtns();
}

void IRAM_ATTR UP_ISR()
{
  Serial.println("ISR");
  global_manager->bt_up_status = digitalRead(BT_UP_PIN);
  if (global_manager->bt_up_status)
    global_manager->context.v_counter++;
}

void IRAM_ATTR DOWN_ISR()
{
  Serial.println("ISR");
  global_manager->bt_down_status = digitalRead(BT_DOWN_PIN);
  if (global_manager->bt_down_status)
    global_manager->context.v_counter--;
}
void IRAM_ATTR LEFT_ISR()
{
  Serial.println("ISR");
  global_manager->bt_left_status = digitalRead(BT_LEFT_PIN);
  if (global_manager->bt_left_status)
    global_manager->context.h_counter--;
}
void IRAM_ATTR RIGHT_ISR()
{
  Serial.println("ISR");
  global_manager->bt_right_status = digitalRead(BT_RIGHT_PIN);
  if (global_manager->bt_right_status)
    global_manager->context.h_counter++;
}
void IRAM_ATTR CENTER_ISR()
{
  Serial.println("ISR");
  global_manager->bt_center_status = digitalRead(BT_CENTER_PIN);
  global_manager->confirm_counter++;
}
void IRAM_ATTR PEDE_ISR()
{
  Serial.println("ISR");
  global_manager->bt_pede_status = digitalRead(BT_PEDE_PIN);
}

void Manager::initBtns()
{
  pinMode(BT_UP_PIN, INPUT_PULLUP);
  pinMode(BT_DOWN_PIN, INPUT_PULLUP);
  pinMode(BT_LEFT_PIN, INPUT_PULLUP);
  pinMode(BT_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BT_CENTER_PIN, INPUT_PULLUP);
  pinMode(BT_PEDE_PIN, INPUT_PULLUP);
  attachInterrupt(BT_UP_PIN, UP_ISR, CHANGE);
  attachInterrupt(BT_DOWN_PIN, DOWN_ISR, CHANGE);
  attachInterrupt(BT_LEFT_PIN, LEFT_ISR, CHANGE);
  attachInterrupt(BT_RIGHT_PIN, RIGHT_ISR, CHANGE);
  attachInterrupt(BT_CENTER_PIN, CENTER_ISR, CHANGE);
  attachInterrupt(BT_PEDE_PIN, PEDE_ISR, CHANGE);
}

void Manager::pushMenu(Menu *m)
{
  menu_stack.push(m);
}

void Manager::showMenu()
{
  // Serial.printf("var1: %d\n", context.v_counter);
  // Serial.printf("var2: %d\n", context.h_counter);
  context.go_back = -1;
  context.next_menu = -1;
  context.enter_counter = confirm_counter;
  // Menu atual tambem será passado para função de callback
  context.current_menu = menu_stack.top();

  menu_stack.top()->show(&context);

  // Verifica se holve uma ação de avançar para o proximo submenu
  if (context.next_menu >= 0)
  {
    Serial.printf("next\n");
    if (context.go_back >= 0)
    {
      Serial.printf("goback\n");
      //Remove o menu de goback e o menu anterior para que o retorno ao menu funcione corretamente
      menu_stack.pop();
      menu_stack.pop();
    }
    else
      menu_stack.push(menu_stack.top()->sub_menus[context.next_menu]);
  }
  // verifica se holve uma ação para retornar ao menu anterior
  Serial.printf("Tamanho da pilha de menus: %d\n", menu_stack.size());
  // Limpa variavel referente a contagem de interrupções do botão de confirmação (botão central)
  confirm_counter = 0;
}
