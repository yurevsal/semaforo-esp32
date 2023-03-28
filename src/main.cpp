#include "defines.h"
#include <nvs_flash.h>

// #ifdef CONFIG_ARDUINO_LOOP_STACK_SIZE
// #undef CONFIG_ARDUINO_LOOP_STACK_SIZE
// #define CONFIG_ARDUINO_LOOP_STACK_SIZE 16000
// #endif

namespace keyboard
{
  enum e
  {
    PRESSED,
    RELEASED
  };
}
namespace semaphore
{
  enum e
  {
    NORMAL,
    HIGHWAY,
    BLINK
  };
}
namespace colors
{
  enum e
  {
    GREEN = 2,
    YELLOW = 1,
    RED = 0,
    OFF
  };
}

// Inicializa o display OLED
// ADDRESS, SDA, SCL
// TwoWire I2C1 = TwoWire(0);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_64, I2C_ONE, 100000);

// ESP32Time rtc;
ESP32Time esp32_rtc; // offset in seconds GMT+3

// Objeto referente ao RTC
RTC_DS1307 rtc;
String hours_str;

// Cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

WiFiManager wm;

std::vector<Plan_model> plan_list;

void top_bar(String msg1, String time, OLEDDISPLAY_TEXT_ALIGNMENT align);
void body_box(std::vector<String> options, int option);
void home_box(std::vector<String> options);
void printCurrentTime(int num);
void savePlan(int n);
void saveClock(int n);
void setPlan(int _plan);
int getPlainIdx();
void getTimeNTP(int n);
void cleanWifi(int n);
void updateClockVars();
void getStartTimePlan(unsigned long *events);
void saveBlinkPlan(int n);

int plan = 1, last_plan = 1;
Plan_model current_plan;
int time_green = 1, time_yellow = 1, time_red = 1;
int day = 1, month = 1, year = 2020, hour = 1, minute = 1;
int current_channel;
String current_color, pede_current_color;
int regressive_counter_GREEN = 0;
int regressive_counter_YELLOW = 0;
int regressive_counter_RED = 0;
unsigned long horarios[N_PLAN_MAX + 1];

void Manager::showMenu()
{
  if (!sub_menus.empty())
  {
    static uint32_t counter;
    // Obtem o titulo do menu.
    String title = current_menu()->getTitle();
    // Obtem strings de cada opção.
    for (int i = 0; i < current_menu()->subMenusLenght(); i++)
    {
      options.push_back(current_menu()->getSubMenuTitle(i));
    }
    // int index = increase_option;
    int index = current_menu()->getOptionIdx();
    // top_bar(title, hours_str); // Alterar o acesso á esse rtc
    // Mostra o Menu
    if (millis() - counter >= REFRESH_DELAY)
    {
      // Serial.printf("idle: %d\n",idle);
      counter = millis();
      refreshScreen = true;
    }
    if (refreshScreen)
    {
      refreshScreen = false;
      display.clear();
      top_bar(NOME_EMPRESA, hours_str, TEXT_ALIGN_CENTER);
      if ((millis() - menuIdleCounterTime) / 1000 > IDLE_MENU_LIMIT)
      {
        // Menu oscioso
        idle = true;
        vector<String> lines;
        // if(current_plan.type == semaphore::NORMAL)
        // {
        if (current_color == "VD")
          regressive_counter_GREEN -= REFRESH_DELAY;
        if (current_color == "AM")
          regressive_counter_YELLOW -= REFRESH_DELAY;
        if (current_color == "VM")
          regressive_counter_RED -= REFRESH_DELAY;

        if (regressive_counter_GREEN < 0)
          regressive_counter_GREEN = 0;
        if (regressive_counter_YELLOW < 0)
          regressive_counter_YELLOW = 0;
        if (regressive_counter_RED < 0)
          regressive_counter_RED = 0;

        String aux = "Estagio: {FASE}      PLN: {PLAN}";
        aux.replace("{FASE}", String(current_channel));
        aux.replace("{PLAN}", String(plan));
        // aux.replace("{TIME}",String(regressive_counter/1000+1));
        lines.push_back(aux);
        aux = "Tempo Vermelho: {TIME}";
        aux.replace("{TIME}", String(regressive_counter_RED / 1000));
        lines.push_back(aux);
        aux = "Tempo Amarelo: {TIME}";
        aux.replace("{TIME}", String(regressive_counter_YELLOW / 1000));
        lines.push_back(aux);
        aux = "Tempo Verde: {TIME}";
        aux.replace("{TIME}", String(regressive_counter_GREEN / 1000));
        lines.push_back(aux);
        // }
        // if(current_plan.type == semaphore::HIGHWAY)
        // {
        //   lines.push_back("");
        //   lines.push_back("       Alta Demanda!");
        //   lines.push_back("");
        //   lines.push_back("");
        // }
        // if(current_plan.type == semaphore::BLINK)
        // {
        //   lines.push_back("");
        //   lines.push_back("  Amarelo intermitente!");
        //   lines.push_back("");
        //   lines.push_back("");
        // }
        home_box(lines);
      }
      else
      {
        // Menu ativo
        top_bar("Configurações", "", TEXT_ALIGN_CENTER);
        body_box(options, index);
      }
      display.display();
    }
    options.clear();
  }
  else
    Serial.println("Outra opção");
}
Manager screen_manager = Manager();
Menu main_menu("Plano {0}", 0, &plan),
    menu_plans("Planos"),
    menu_ped("Tipo: {0}", 0, &current_plan.type, 0, 1, {"CICLICO", "DEMANDA", "AMARELO"}),
    menu_config_fases("Config. Fases"),
    blink_y("Amarelo intermitente"),
    menu_RTC("Ajustar RTC"),
    menu_NTP("NTP Server"),
    menu_wifi_off("Desconectar WiFi");

Menu start_time("Hora de inicio"),
    end_time("Hora de termino");
    // select_day("Dia de inicio/fim");
Menu y_start_time("Hora de inicio"),
    y_end_time("Hora de termino");
Menu get_start_hour("Hora: {0}", 0, &current_plan.start_hour, 0, 23),
    get_start_min("Minuto: {0}", 0, &current_plan.start_min, 0, 59),
    get_end_hour("Hora: {0}", 0, &current_plan.end_hour, 0, 23),
    get_end_min("Minuto: {0}", 0, &current_plan.end_min, 0, 59),
    get_start_day("Inicio: {0}", 0, &current_plan.start_day, 1, 7),
    get_end_day("Final: {0}", 0, &current_plan.end_day, 1, 7);

Menu menu_plan_index("Plano: {0}", 0, &plan, 1, N_PLAN_MAX),
    menu_phases_length("N° fases: {0}", 0, &current_plan.n_phases, 1, N_CANAIS_MAX),
    menu_plan_time("Horario"),
    fase1("Fase 1"),
    fase2("Fase 2"),
    fase3("Fase 3"),
    fase4("Fase 4"),
    fase5("Fase 5"),
    fase6("Fase 6"),
    fase7("Fase 7"),
    fase8("Fase 8");

Menu t_green1("Tempo VD: {0}", 0, &current_plan.delays[0][0], 0, 999),
    t_yellow1("Tempo AM: {0}", 0, &current_plan.delays[0][1], 0, 999),
    t_red1("Tempo VM: {0}", 0, &current_plan.delays[0][2], 0, 999),
    t_green2("Tempo VD: {0}", 0, &current_plan.delays[1][0], 0, 999),
    t_yellow2("Tempo AM: {0}", 0, &current_plan.delays[1][1], 0, 999),
    t_red2("Tempo VM: {0}", 0, &current_plan.delays[1][2], 0, 999),
    t_green3("Tempo VD: {0}", 0, &current_plan.delays[2][0], 0, 999),
    t_yellow3("Tempo AM: {0}", 0, &current_plan.delays[2][1], 0, 999),
    t_red3("Tempo VM: {0}", 0, &current_plan.delays[2][2], 0, 999),
    t_green4("Tempo VD: {0}", 0, &current_plan.delays[3][0], 0, 999),
    t_yellow4("Tempo AM: {0}", 0, &current_plan.delays[3][1], 0, 999),
    t_red4("Tempo VM: {0}", 0, &current_plan.delays[3][2], 0, 999),
    t_green5("Tempo VD: {0}", 0, &current_plan.delays[4][0], 0, 999),
    t_yellow5("Tempo AM: {0}", 0, &current_plan.delays[4][1], 0, 999),
    t_red5("Tempo VM: {0}", 0, &current_plan.delays[4][2], 0, 999),
    t_green6("Tempo VD: {0}", 0, &current_plan.delays[5][0], 0, 999),
    t_yellow6("Tempo AM: {0}", 0, &current_plan.delays[5][1], 0, 999),
    t_red6("Tempo VM: {0}", 0, &current_plan.delays[5][2], 0, 999),
    t_green7("Tempo VD: {0}", 0, &current_plan.delays[6][0], 0, 999),
    t_yellow7("Tempo AM: {0}", 0, &current_plan.delays[6][1], 0, 999),
    t_red7("Tempo VM: {0}", 0, &current_plan.delays[6][2], 0, 999),
    t_green8("Tempo VD: {0}", 0, &current_plan.delays[7][0], 0, 999),
    t_yellow8("Tempo AM: {0}", 0, &current_plan.delays[7][1], 0, 999),
    t_red8("Tempo VM: {0}", 0, &current_plan.delays[7][2], 0, 999);

Menu date_day("Dia: {0}", 0, &day, 1, 31),
    date_month("Mes: {0}", 0, &month, 1, 12),
    date_year("Ano: {0}", 0, &year, 2020, 2100),
    date_hour("Horas: {0}", 0, &hour, 0, 23),
    date_min("Minutos: {0}", 0, &minute, 0, 59);

Menu _back("Voltar", 1);
Menu _save_times("Voltar", 1);
Menu _save_clock("Salvar e Voltar", 1);
Menu _save_back("Salvar e Voltar", 1, &plan, 0, 6);
Menu _save_yellow("Salvar e Voltar", 1);
Menu ntp_back("Sincronizar horário", 1);
Menu clear_wifi_back("Apagar e Voltar", 1);

void initMenus()
{
  // current_plan.n_phases = 1;
  // current_plan.type = semaphore::NORMAL; // HIGHWAY; // Define o modo de funcionamento com relação ao semaforo do pedestre
  // fase1.pushMenu(&_save_times, &saveTimes);
  fase1.pushMenu(&_save_times);
  fase1.pushMenu(&t_red1);
  fase1.pushMenu(&t_yellow1);
  fase1.pushMenu(&t_green1);

  fase2.pushMenu(&_save_times, &printCurrentTime);
  fase2.pushMenu(&t_red2);
  fase2.pushMenu(&t_yellow2);
  fase2.pushMenu(&t_green2);

  fase3.pushMenu(&_save_times);
  fase3.pushMenu(&t_red3);
  fase3.pushMenu(&t_yellow3);
  fase3.pushMenu(&t_green3);

  fase4.pushMenu(&_save_times);
  fase4.pushMenu(&t_red4);
  fase4.pushMenu(&t_yellow4);
  fase4.pushMenu(&t_green4);

  fase5.pushMenu(&_save_times);
  fase5.pushMenu(&t_red5);
  fase5.pushMenu(&t_yellow5);
  fase5.pushMenu(&t_green5);

  fase6.pushMenu(&_save_times);
  fase6.pushMenu(&t_red6);
  fase6.pushMenu(&t_yellow6);
  fase6.pushMenu(&t_green6);

  fase7.pushMenu(&_save_times);
  fase7.pushMenu(&t_red7);
  fase7.pushMenu(&t_yellow7);
  fase7.pushMenu(&t_green7);

  fase8.pushMenu(&_save_times);
  fase8.pushMenu(&t_red8);
  fase8.pushMenu(&t_yellow8);
  fase8.pushMenu(&t_green8);

  // menu_config_fases.pushMenu(&_back);
  menu_config_fases.pushMenu(&_save_back, &savePlan);
  menu_config_fases.pushMenu(&fase1);
  menu_config_fases.pushMenu(&fase2);
  menu_config_fases.pushMenu(&fase3);
  menu_config_fases.pushMenu(&fase4);
  menu_config_fases.pushMenu(&fase5);
  menu_config_fases.pushMenu(&fase6);
  menu_config_fases.pushMenu(&fase7);
  menu_config_fases.pushMenu(&fase8);

  // start_time.pushMenu(&_back);
  start_time.pushMenu(&_save_back, &savePlan);
  start_time.pushMenu(&get_start_hour);
  start_time.pushMenu(&get_start_min);

  // end_time.pushMenu(&_back);
  end_time.pushMenu(&_save_back, &savePlan);
  end_time.pushMenu(&get_end_hour);
  end_time.pushMenu(&get_end_min);

  // select_day.pushMenu(&_back);
  // select_day.pushMenu(&get_start_day);
  // select_day.pushMenu(&get_end_day);

  menu_plan_time.pushMenu(&_back);
  menu_plan_time.pushMenu(&_save_back, &savePlan);
  menu_plan_time.pushMenu(&start_time);
  // menu_plan_time.pushMenu(&end_time);
  // menu_plan_time.pushMenu(&select_day);

  // menu_plans.pushMenu(&_back);
  menu_plans.pushMenu(&_save_back, &savePlan);
  menu_plans.pushMenu(&menu_plan_index);
  menu_plans.pushMenu(&menu_ped);
  menu_plans.pushMenu(&menu_phases_length);
  menu_plans.pushMenu(&menu_config_fases);
  menu_plans.pushMenu(&menu_plan_time);

  // menu_RTC.pushMenu(&_back);
  menu_RTC.pushMenu(&_save_clock, &saveClock);
  menu_RTC.pushMenu(&date_day);
  menu_RTC.pushMenu(&date_month);
  menu_RTC.pushMenu(&date_year);
  menu_RTC.pushMenu(&date_hour);
  menu_RTC.pushMenu(&date_min);

  menu_NTP.pushMenu(&_back);
  menu_NTP.pushMenu(&ntp_back, &getTimeNTP);

  // menu_save.pushMenu(&_back);
  // menu_save.pushMenu(&_save_back, &savePlan);

  menu_wifi_off.pushMenu(&_back);
  menu_wifi_off.pushMenu(&clear_wifi_back, &cleanWifi);

  
    // start_time.pushMenu(&_back);
  y_start_time.pushMenu(&_save_yellow, &saveBlinkPlan);
  y_start_time.pushMenu(&get_start_hour);
  y_start_time.pushMenu(&get_start_min);

  // end_time.pushMenu(&_back);
  y_end_time.pushMenu(&_save_yellow, &saveBlinkPlan);
  y_end_time.pushMenu(&get_end_hour);
  y_end_time.pushMenu(&get_end_min);
  // blink_y.pushMenu(&_back);
  
  blink_y.pushMenu(&_save_yellow, &saveBlinkPlan);
  blink_y.pushMenu(&menu_phases_length);
  blink_y.pushMenu(&y_start_time);
  blink_y.pushMenu(&y_end_time);

  main_menu.pushMenu(&menu_plans);
  main_menu.pushMenu(&blink_y);
  main_menu.pushMenu(&menu_RTC);
  main_menu.pushMenu(&menu_NTP);
  main_menu.pushMenu(&menu_wifi_off);
  screen_manager.pushMenu(&main_menu);
}

PCF8574 canal1(0x27, SDA, SCL);
PCF8574 canal2(0x23, SDA, SCL);
PCF8574 canal3(0x21, SDA, SCL);
PCF8574 canal4(0x20, SDA, SCL);

void setFase(int fase, int color, int value)
{
  if (!(fase % 2))
    color += 3;
  switch (fase)
  {
  case 1:
    canal1.digitalWrite(color, value);
    break;
  case 2:
    canal1.digitalWrite(color, value);
    break;
  case 3:
    canal2.digitalWrite(color, value);
    break;
  case 4:
    canal2.digitalWrite(color, value);
    break;
  case 5:
    canal3.digitalWrite(color, value);
    break;
  case 6:
    canal3.digitalWrite(color, value);
    break;
  case 7:
    canal4.digitalWrite(color, value);
    break;
  case 8:
    canal4.digitalWrite(color, value);
    break;
  }
}

uint8_t bt_up = keyboard::RELEASED;
uint8_t bt_down = keyboard::RELEASED;
uint8_t bt_left = keyboard::RELEASED;
uint8_t bt_right = keyboard::RELEASED;
uint8_t bt_center = keyboard::RELEASED;
uint8_t bt_pede = keyboard::RELEASED;
bool key_clicked = false;
bool pede_clicked = false;
unsigned long refreshServerTime = 5;
unsigned long corrigeRTC = 0;
const char *serverName = "https://semapho.herokuapp.com/api2";

void top_bar(String msg1, String time, OLEDDISPLAY_TEXT_ALIGNMENT align)
{
  int offset = 0;
  if (align == TEXT_ALIGN_CENTER)
    offset = 128 / 2;
  display.setFont(ArialMT_Plain_10);
  display.fillRect(0, 0, 128, 15);
  display.setColor(BLACK);
  display.setTextAlignment(align);
  display.drawString(offset + 1, 0, msg1);
  // display.setTextAlignment(TEXT_ALIGN_CENTER);
  // if(WiFi.status() == WL_CONNECTED){
  //   time = time + "/ON";
  // }
  // else{
  //   time = time + "/OFF";
  // }
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(127, 0, time);
  display.setColor(WHITE);
}

void body_box(std::vector<String> options, int option)
{
  int first_option = (option > 3) ? option - 3 : 0;
  int nlines = min(int(options.size()), 4);
  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i = first_option, j = 16; i < first_option + nlines; i++, j += 10)
  {
    if (i == option)
      options[i] = ">" + options[i];
    else
      options[i] = " " + options[i];
    display.drawString(1, j, options[i]);
  }
  // display.drawRect(0, 16, 128, 48);
}

void home_box(std::vector<String> options)
{
  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  for (int i = 0, j = 16; i < options.size(); i++, j += 10)
  {
    display.drawString(1, j, options[i]);
  }
  // display.drawRect(0, 16, 128, 48);
  updateClockVars();
}

void beginScreen(String title)
{
  uint8_t width = 128;
  display.setFont(ArialMT_Plain_10);
  display.fillRect(0, 0, width, 15);
  display.setColor(BLACK);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(width / 2, 0, title);
  display.setColor(WHITE);
  display.drawString(width / 2, 20, "Semaforo Iniciando");
  display.drawString(width / 2, 30, "Aguarde...");
  display.setColor(WHITE);
}

String getId(String prefix, int n)
{
  char id[23];
  int begin = 9;

  if (n < 13 && n > 0)
    begin += n;
  else
    begin = 23;

  snprintf(id, begin, "%s%llX", prefix.c_str(), ESP.getEfuseMac());
  return String(id);
}
/****************************************************************/
bool initRTC()
{
  if (!rtc.begin(&Wire))
  {
    Serial.println("Couldn't find RTC");
    return false;
  }
  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    return false;
  }
  return true;
}
// Inicialização das saidas para leds
// São utilizados apenas 6 bits de cada chip
void initOutputs()
{
  for (int i = 0; i < 6; i++)
  {
    canal1.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal2.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal3.pinMode(i, OUTPUT, 0);
  }
  for (int i = 0; i < 6; i++)
  {
    canal4.pinMode(i, OUTPUT, 0);
  }
  canal1.begin();
  canal2.begin();
  canal3.begin();
  canal4.begin();
}
// Inicialização das entradas digitais necessarias
void initInputs()
{
  pinMode(BT_UP, INPUT_PULLUP);
  pinMode(BT_DOWN, INPUT_PULLUP);
  pinMode(BT_LEFT, INPUT_PULLUP);
  pinMode(BT_RIGHT, INPUT_PULLUP);
  pinMode(BT_CENTER, INPUT_PULLUP);
  pinMode(BT_PEDE, INPUT_PULLUP);
}
// Pooling para os botões
void switchPolling(void *parameter)
{
  pinMode(2, OUTPUT);
  esp_task_wdt_add(NULL);
  for (;;)
  {
    digitalWrite(2,
                 !digitalRead(BT_UP) ||
                     !digitalRead(BT_DOWN) ||
                     !digitalRead(BT_LEFT) ||
                     !digitalRead(BT_RIGHT) ||
                     !digitalRead(BT_CENTER) ||
                     !digitalRead(BT_PEDE));
    if (!digitalRead(BT_UP))
    {
      vTaskDelay(250 / portTICK_PERIOD_MS);
      bt_up = keyboard::PRESSED;
    }
    else
    {
      if (bt_up == keyboard::PRESSED)
      {
        // retorna uma posição no menu.
        screen_manager.goBackOption();
        bt_up = keyboard::RELEASED;
        key_clicked = true;
      }
    }
    if (!digitalRead(BT_DOWN))
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      bt_down = keyboard::PRESSED;
    }
    else
    {
      if (bt_down == keyboard::PRESSED)
      {
        // Avança para a proxima opção no menu.
        screen_manager.nextOption();
        bt_down = keyboard::RELEASED;
        key_clicked = true;
      }
    }
    if (!digitalRead(BT_LEFT))
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      bt_left = keyboard::PRESSED;
    }
    else
    {
      if (bt_left == keyboard::PRESSED)
      {
        // Incrementa uma variavel que o menu controla
        screen_manager.decreaseOption();
        bt_left = keyboard::RELEASED;
        key_clicked = true;
      }
    }
    if (!digitalRead(BT_RIGHT))
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      bt_right = keyboard::PRESSED;
    }
    else
    {
      if (bt_right == keyboard::PRESSED)
      {
        // Incrementa uma variavel que o menu controla
        screen_manager.increaseOption();
        bt_right = keyboard::RELEASED;
        key_clicked = true;
      }
    }
    if (!digitalRead(BT_CENTER))
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      bt_center = keyboard::PRESSED;
    }
    else
    {
      if (bt_center == keyboard::PRESSED)
      {
        key_clicked = true;
        bt_center = keyboard::RELEASED;
        screen_manager.nextSubMenu();
      }
    }
    if (!digitalRead(BT_PEDE))
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      bt_pede = keyboard::PRESSED;
    }
    else
    {
      if (bt_pede == keyboard::PRESSED)
      {
        bt_pede = keyboard::RELEASED;
        pede_clicked = true;
      }
    }
    esp_task_wdt_reset();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void ledsLoop(void *parameter)
{
  esp_task_wdt_add(NULL);
  // current_plan = getTimes(0);
  // Amarelo intermitente inicial
  for (int i = 0; i < 3; i++)
  {
    // current_plan.n_phases = 4;
    for (int i = 1; i <= 4; i++)
    {
      esp_task_wdt_reset();
      setFase(i, colors::YELLOW, 1);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); //
    for (int i = 1; i <= 7; i++)
    {
      esp_task_wdt_reset();
      setFase(i, colors::YELLOW, 0);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); //
  }
  // Vermelhos 3 segundos
  for (int i = 1; i <= 4; i++)
  {
    esp_task_wdt_reset();
    setFase(i, colors::RED, 1);
  }
  vTaskDelay(3000 / portTICK_PERIOD_MS);
  for (;;)
  {
    esp_task_wdt_reset();
    // Modo via comum
    // Inicializando semaforos
    for (int i = 1; i <= current_plan.n_phases; i++)
    {
      esp_task_wdt_reset();
      setFase(i, colors::GREEN, 0);
      setFase(i, colors::YELLOW, 0);
      setFase(i, colors::RED, 1);
    }

    while (current_plan.type == semaphore::NORMAL)
    {
      esp_task_wdt_reset();
      int canal_counter = 0;
      bool canal_turn_off = false;
      // Checa o numero de canals ajustado pelo usuario
      // if(canal_counter >= current_plan.n_phases)
      // {
      //   canal_turn_off = true;
      // }
      // canal_counter ++;
      for (int i = 1; i <= current_plan.n_phases; i++)
      {
        current_channel = i;
        if (pede_clicked)
        {
          pede_current_color = "VD";
          setFase(FASE_PEDE, colors::GREEN, 1);
          setFase(FASE_PEDE, colors::RED, 0);
          esp_task_wdt_reset();
          vTaskDelay(5000 / portTICK_PERIOD_MS);
          pede_current_color = "VM";
          setFase(FASE_PEDE, colors::GREEN, 0);
          setFase(FASE_PEDE, colors::RED, 1);
          esp_task_wdt_reset();
          vTaskDelay(100 / portTICK_PERIOD_MS);
          pede_clicked = false;
        }
        else
        {
          pede_current_color = "VM";
          setFase(FASE_PEDE, colors::GREEN, 0);
          setFase(FASE_PEDE, colors::YELLOW, 0);
          setFase(FASE_PEDE, colors::RED, 1);
        }

        regressive_counter_GREEN = current_plan.delays[i - 1][colors::GREEN] * 1000;
        regressive_counter_YELLOW = current_plan.delays[i - 1][colors::YELLOW] * 1000;
        regressive_counter_RED = current_plan.delays[i - 1][colors::RED] * 1000;
        // Semaforo verde
        current_color = "VD";
        setFase(i, colors::GREEN, 1);
        setFase(i, colors::YELLOW, 0);
        setFase(i, colors::RED, 0);
        esp_task_wdt_reset();
        vTaskDelay(current_plan.delays[i - 1][colors::GREEN] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos

        // Semaforo amarelo
        current_color = "AM";
        setFase(i, colors::GREEN, 0);
        setFase(i, colors::YELLOW, 1);
        setFase(i, colors::RED, 0);
        esp_task_wdt_reset();
        vTaskDelay(current_plan.delays[i - 1][colors::YELLOW] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos

        // Semaforo vermelho
        current_color = "VM";
        setFase(i, colors::GREEN, 0);
        setFase(i, colors::YELLOW, 0);
        setFase(i, colors::RED, 1);
        esp_task_wdt_reset();
        vTaskDelay(current_plan.delays[i - 1][colors::RED] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
      }
      for (int i = 8; i > current_plan.n_phases; i--)
      {
        setFase(i, colors::GREEN, 0);
        setFase(i, colors::YELLOW, 0);
        setFase(i, colors::RED, 0);
      }
    }
    for (int i = 1; i <= current_plan.n_phases; i++)
    {
      esp_task_wdt_reset();
      setFase(i, colors::GREEN, 0);
      setFase(i, colors::YELLOW, 0);
      setFase(i, colors::RED, 0);
    }
    while (current_plan.type == semaphore::HIGHWAY)
    {
      esp_task_wdt_reset();
      current_channel = 0;
      for (int i = 1; i < 7; i++)
      {
        esp_task_wdt_reset();
        setFase(i, colors::GREEN, 0);
        setFase(i, colors::YELLOW, 0);
        setFase(i, colors::RED, 0);
      }
      // Verde do pedestre aceso
      setFase(FASE_PEDE, colors::GREEN, 0);
      setFase(FASE_PEDE, colors::YELLOW, 0);
      setFase(FASE_PEDE, colors::RED, 1);
      // Primeira fase semaforo dos Carros
      current_color = "VD";
      setFase(1, colors::GREEN, 1);
      setFase(1, colors::YELLOW, 0);
      setFase(1, colors::RED, 0);
      while (!pede_clicked)
      {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
      }
      if (pede_clicked)
      {
        // Acende amarelo
        esp_task_wdt_reset();
        current_color = "AM";
        vTaskDelay(current_plan.delays[0][colors::GREEN] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
        setFase(1, colors::GREEN, 0);
        setFase(1, colors::YELLOW, 1);
        setFase(1, colors::RED, 0);

        // Acende vermelho
        esp_task_wdt_reset();
        vTaskDelay(current_plan.delays[0][colors::YELLOW] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
        setFase(1, colors::GREEN, 0);
        setFase(1, colors::YELLOW, 0);
        setFase(1, colors::RED, 1);
        vTaskDelay(current_plan.delays[0][colors::RED] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
        // Verde dos pedestres
        esp_task_wdt_reset();
        pede_current_color = "VD";
        setFase(FASE_PEDE, colors::GREEN, 1);
        setFase(FASE_PEDE, colors::YELLOW, 0);
        setFase(FASE_PEDE, colors::RED, 0);
        vTaskDelay(current_plan.delays[7][colors::GREEN] * 1000 / portTICK_PERIOD_MS);

        esp_task_wdt_reset();
        pede_current_color = "VM";
        setFase(FASE_PEDE, colors::GREEN, 0);
        setFase(FASE_PEDE, colors::YELLOW, 0);
        setFase(FASE_PEDE, colors::RED, 1);

        // Pisca o vermelho dos pedestres
        esp_task_wdt_reset();
        for (int i = 0; i <= current_plan.delays[7][colors::RED]; i++)
        {
          vTaskDelay(500 / portTICK_PERIOD_MS);
          setFase(FASE_PEDE, colors::RED, 1);
          vTaskDelay(500 / portTICK_PERIOD_MS);
          setFase(FASE_PEDE, colors::RED, 0);
        }
        pede_clicked = false;
      }
    }
    for (int i = 1; i <= current_plan.n_phases; i++)
    {
      esp_task_wdt_reset();
      setFase(i, colors::GREEN, 0);
      setFase(i, colors::YELLOW, 1);
      setFase(i, colors::RED, 0);
    }
    while (current_plan.type == semaphore::BLINK)
    {
      for (int i = 1; i < 7; i++)
      {
        esp_task_wdt_reset();
        setFase(i, colors::RED, 0);
        setFase(i, colors::YELLOW, 0);
        setFase(i, colors::GREEN, 0);
      }
      current_color = "AM";
      current_channel = 0;
      for (int i = 1; i <= current_plan.n_phases; i++)
      {
        esp_task_wdt_reset();
        setFase(i, colors::YELLOW, 1);
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS); //
      for (int i = 1; i <= current_plan.n_phases; i++)
      {
        esp_task_wdt_reset();
        setFase(i, colors::YELLOW, 0);
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS); //
    }
  }
}

// Guarda as configurações de tempo de todas as fases
void savePlan(int n)
{
  Serial.printf("Salvando plano: %d\n", plan);
  Serial.printf("Salvando plano: %d\n", plan_list[plan].id);
  current_plan.id = plan;
  plan_list[plan] = current_plan;
  plan_list[plan].save();
  getStartTimePlan(horarios);
}

void saveClock(int n)
{
  // Ajuste do dia de acordo com o mês e se o ano é bisexto
  if (month == 4 || month == 6 || month == 9 || month == 11)
    day = min(day, 30);
  if (month == 2 && !(year % 4))
  {
    if ((year % 100))
      day = min(day, 29);
    else
      day = min(day, 28);
  }
  // Ajuste dos RTCs
  rtc.adjust(DateTime(year, month, day, hour, minute, 0));
  DateTime now = rtc.now();
  esp32_rtc.setTime(0, minute, hour, day, month, year);
}

void printCurrentTime(int num = 0)
{
  DateTime now = rtc.now();
  char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void wifiSetup(void *parameter)
{
  // esp_task_wdt_add(NULL);
  // esp_task_wdt_reset();
  // wm.setClass("invert"); // dark theme
  wm.setScanDispPerc(true); // display percentages instead of graphs for RSSI
  // wm.resetSettings();
  bool res = wm.autoConnect(); // password protected ap
  // bool res = wm.startConfigPortal();
  if (!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("connected...");
  }
  Serial.println("Encerrando Task");
  vTaskDelete(NULL);
}

void cleanWifi(int n)
{
  wm.resetSettings();
}

void subAllTopics()
{
  client.subscribe("n_plano", 1);
  client.subscribe("n_fases", 1);
  client.subscribe("d_inicio", 1);
  client.subscribe("d_fim", 1);
  client.subscribe("h_inicio", 1);
  client.subscribe("h_fim", 1);
  client.subscribe("vd_t1", 1);
  client.subscribe("vd_t2", 1);
  client.subscribe("vd_t3", 1);
  client.subscribe("vd_t4", 1);
  client.subscribe("vd_t5", 1);
  client.subscribe("vd_t6", 1);
  client.subscribe("vd_t7", 1);
  client.subscribe("vd_t8", 1);

  client.subscribe("am_t1", 1);
  client.subscribe("am_t2", 1);
  client.subscribe("am_t3", 1);
  client.subscribe("am_t4", 1);
  client.subscribe("am_t5", 1);
  client.subscribe("am_t6", 1);
  client.subscribe("am_t7", 1);
  client.subscribe("am_t8", 1);

  client.subscribe("vm_t1", 1);
  client.subscribe("vm_t2", 1);
  client.subscribe("vm_t3", 1);
  client.subscribe("vm_t4", 1);
  client.subscribe("vm_t5", 1);
  client.subscribe("vm_t6", 1);
  client.subscribe("vm_t7", 1);
  client.subscribe("vm_t8", 1);
}

void mqttSetup()
{
  // client.setServer(SERVER, PORT);
  // client.setCallback(callback);
  // client.publish(PUB_TOPIC, "teste");
}

void mqttReconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ifgunxgo", USER, PASS))
    {
      Serial.println("connected");
      // Subscribe
      subAllTopics();
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void getTimeNTP(int n)
{
  if (WiFi.status() != WL_CONNECTED)
    return;
  WiFiUDP udp;
  NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000); /* Cria um objeto "NTP" com as configurações.utilizada no Brasil */
  ntp.begin();                                          /* Inicia o protocolo */
  ntp.forceUpdate();                                    /* Atualização */
  String formattedDate = ntp.getFormattedDate();

  // Extract date
  int split_T = formattedDate.indexOf("T");
  int split_ = formattedDate.indexOf("-");
  String dayStamp = formattedDate.substring(0, split_T);
  String day, month, year;
  year = dayStamp.substring(0, split_);
  dayStamp.replace(year + "-", "");
  split_ = dayStamp.indexOf("-");

  month = dayStamp.substring(0, split_);
  dayStamp.replace(month + "-", "");
  split_ = dayStamp.indexOf("-");

  day = dayStamp.substring(0, split_);

  // Serial.printf("\n%d %d %d\n", day.toInt(), month.toInt(), year.toInt());

  // Extract time
  String timeStamp = formattedDate.substring(split_T + 1, formattedDate.length() - 1);

  int split_dp = timeStamp.indexOf(":");
  String hour, _min, sec;

  hour = timeStamp.substring(0, split_dp);
  timeStamp.replace(hour + ":", "");
  split_dp = timeStamp.indexOf(":");

  _min = timeStamp.substring(0, split_dp);
  timeStamp.replace(_min + ":", "");
  split_dp = timeStamp.indexOf(":");

  sec = timeStamp.substring(0, split_dp);
  // Serial.printf("\n%d %d %d\n", hour.toInt(), _min.toInt(), sec.toInt());
  rtc.adjust(DateTime(year.toInt(), month.toInt(), day.toInt(), hour.toInt(), _min.toInt(), sec.toInt()));
  DateTime now = rtc.now();
  esp32_rtc.setTime(now.unixtime());
  Serial.println("Correção de horario via NTP");
}

void updateClockVars()
{
  minute = esp32_rtc.getMinute();
  hour = esp32_rtc.getHour(true);
  day = esp32_rtc.getDay();
  month = esp32_rtc.getMonth();
  year = esp32_rtc.getYear();
}

bool isWithinRange(unsigned long startTime, unsigned long endTime, unsigned long timeToCheck)
{
  if (startTime <= endTime)
  { // caso normal, sem passagem de meia-noite
    return (timeToCheck >= startTime && timeToCheck <= endTime);
  }
  else
  { // caso com passagem de meia-noite
    return (timeToCheck >= startTime || timeToCheck <= endTime);
  }
}

// Rotina que verifica horarios dos planos
void timerEvents(void *parameter)
{
  esp_task_wdt_add(NULL);
  for (;;)
  {
    if (screen_manager.idle)
    {
      uint8_t pos = 0;
      unsigned long min_diff = 86400;
      unsigned long active = 0;
      unsigned long active_time = 0;
      unsigned long now = esp32_rtc.getSecond() + 60 * esp32_rtc.getMinute() + 60 * 60 * esp32_rtc.getHour(true);
      Serial.println("[1] ESTOU VIVO");
      // Serial.printf("[%ld]-[%ld]-[%ld]\n", horarios[0], plan_list[0].end_hour * 3600 + plan_list[0].end_min * 60, now);

      if (isWithinRange(horarios[0], plan_list[0].end_hour * 3600 + plan_list[0].end_min * 60, now))
      {
        current_plan = plan_list[0];
        Serial.printf("Amarelo intermitente. Horário: %dh:%d até as %dh:%d\n",plan_list[0].start_hour,plan_list[0].start_min, plan_list[0].end_hour,plan_list[0].end_min);
      }
      else
      {
      for (uint8_t i = N_PLAN_MAX; i > 0; i--)
      {
        Serial.printf("[%lu] ", horarios[N_PLAN_MAX - i]);
        // Limite inferior
        long diff = (long)horarios[i] - now;
        // se diff for negativo significa que o horario é anterior ao horario atual
        if (diff < 0)
        {
          diff = diff * (-1);
          if (diff <= min_diff)
          {
            min_diff = diff;
            active = i;
            active_time = horarios[i];
          }
        }
      }
      // Condição para quando o horario atual é anterior a todos os eventos
      if(active == 0)
      {
        long temp=0;
        for(uint8_t i = N_PLAN_MAX; i > 0; i--)
        {
          if (temp < horarios[i])
          {
            active = i;
            temp = horarios[i];
          }
        }
      }
      Serial.printf(" Agora: %lu\n", now);
      current_plan = plan_list[active];
      Serial.println(esp32_rtc.getTime("%H:%M"));
      Serial.printf("[GLOBAL]: O plano atual: %d , pedestre tipo: %d, numero de fases: %d, hora: %lu:%lu\n", active, current_plan.type, current_plan.n_phases, current_plan.start_hour, current_plan.start_min);
      if (plan != active)
      {
        plan = active;
        Serial.printf("Mudança de plano: %d\n", plan);
        // current_plan = getTimes(active);
        current_plan = plan_list[active];
        // if (plan == 0)
        // {
        //   current_plan.type = semaphore::BLINK;
        // }

        //   // savePlan(plan);
      }
    }
    }
    else
    {
      Serial.printf("Plano: %d \n", plan);
    }
    esp_task_wdt_reset();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void dumpSystemInfo()
{
  for (int i = 0; i <= N_PLAN_MAX; i++)
  {
    String name = "plano_" + String(i);
    Serial.printf("**************************************************\n");
    Serial.printf("Plano: %s\n", name.c_str());
    Serial.printf("Tipo de pedestre: \t[%d]\n", plan_list[i].type);
    Serial.printf("Dia de inicio:    \t[%d]\n", plan_list[i].start_day);
    Serial.printf("Dia de termino:   \t[%d]\n", plan_list[i].end_day);
    Serial.printf("hora de inicio:   \t[%d]\n", plan_list[i].start_hour);
    Serial.printf("minuto de inicio: \t[%d]\n", plan_list[i].start_min);
    Serial.printf("hora de termino:  \t[%d]\n", plan_list[i].end_hour);
    Serial.printf("minuto de termino:\t[%d]\n", plan_list[i].end_min);
    Serial.printf("hora de inicio:   \t[%d]\n", plan_list[i].start_hour);
    Serial.printf("Numero de fases:  \t[%d]\n", plan_list[i].n_phases);
    Serial.printf("Atrasos entre luzes\n");
    for (int i = 0; i < 8; i++)
    {
      Serial.printf("[Fase %d] => Verde:\t[%d]\n", i, plan_list[i].delays[i][colors::GREEN]);
      Serial.printf("[Fase %d] => Amarelo:\t[%d]\n", i, plan_list[i].delays[i][colors::YELLOW]);
      Serial.printf("[Fase %d] => vermelho:\t[%d]\n", i, plan_list[i].delays[i][colors::RED]);
    }
  }
  // getTimes(getPlainIdx());
}

void getStartTimePlan(unsigned long *events)
{
  Serial.printf("Lista de horarios\n");

  for (int i = 0; i <= N_PLAN_MAX; i++)
  {
    int start_hour = plan_list[i].start_hour;
    int start_min = plan_list[i].start_min;
    // Guarda um horario no array de horarios
    events[i] = (unsigned long)start_hour * 60 * 60 + start_min * 60;
    Serial.printf("event[%d] =\t%ld\n", i, events[i]);
  }
  Serial.printf("***********************\n");
}

void saveBlinkPlan(int n)
{
  // OBS: implementar a quantidade de fases tambem
  Serial.printf("Salvando plano amarelo intermitente\n");
  plan_list[0].type = 2; // Tipo 2 (Amarelo intermitente)
  plan_list[0].n_phases = current_plan.n_phases;
  plan_list[0].start_hour = current_plan.start_hour;
  plan_list[0].start_min = current_plan.start_min;
  plan_list[0].end_hour = current_plan.end_hour;
  plan_list[0].end_min = current_plan.end_min;
  plan_list[0].save();
  getStartTimePlan(horarios);
}

void init_plans()
{
  SPIFFS.begin(false);
  current_plan.listDir();
  for (int i = 0; i <= N_PLAN_MAX; i++)
  {
    Plan_model p;
    p.init(i);
    plan_list.push_back(p);
  }
}

void setup()
{
  Serial.begin(115200);
  // Configura o wdt em 60s
  int ret = esp_task_wdt_init(60, true);
  if (ret == ESP_OK)
    Serial.println("TWDT successfully deinitialized");
  if (ret == ESP_ERR_INVALID_STATE)
    Serial.println("Error, tasks are still subscribed to the TWDT");
  if (ret == ESP_ERR_NOT_FOUND)
    Serial.println("Error, TWDT has already been deinitialized");
  esp_task_wdt_add(NULL);
  Serial.println(CONFIG_ARDUINO_LOOP_STACK_SIZE);
  Serial.println();

  init_plans();

  // I2C1.begin(SDA, SCL, uint32_t(100000));
  Wire.begin(SDA, SCL, uint32_t(100000));
  initOutputs();
  initInputs();
  initMenus();
  initRTC();
  DateTime now = rtc.now();
  esp32_rtc.setTime(now.unixtime());
  getStartTimePlan(horarios);

  xTaskCreate(switchPolling, "switch_polling", 14048, NULL, 1, NULL);
  xTaskCreate(timerEvents, "timerEvents", 4048, NULL, 1, NULL);
  xTaskCreate(ledsLoop, "leds_loop", 4048, NULL, 1, NULL);

  display.init();
  display.flipScreenVertically();
  // display.drawXbm(14, 14, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_NAME);
  // display.display();
  printCurrentTime();
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  beginScreen(NOME_EMPRESA);
  display.display();
  vTaskDelay(4000 / portTICK_PERIOD_MS);

  xTaskCreate(wifiSetup, "wifi_setup", 4048, NULL, 10, NULL);
  // mqttSetup();
  // dumpSystemInfo();
  pinMode(0, INPUT_PULLUP);
}

void loop()
{
  if (digitalRead(0) == 0)
  {
    if (SPIFFS.format())
    {
      Serial.println("\n\nSuccess formatting");
    }
    else
    {
      Serial.println("\n\nError formatting");
    }
    ESP.restart();
  }
  hours_str = esp32_rtc.getTime("%H:%M");
  screen_manager.showMenu();
  // Serial.println(current_plan.type);
  if (key_clicked)
  {
    key_clicked = false;
  }
  // Teste de requisição em 10s
  if ((millis() - refreshServerTime) / 1000 >= 10)
  {
    // Serial.println("Request server");
    // requestServer(serverName);
    // Serial.printf("Refresh server\n");
    if (WiFi.status() == WL_CONNECTED)
    {
      mqttReconnect();
      // Serial.printf("MQTT publish: %d\n", client.publish("/millis", String(millis()/1000).c_str()));
      // client.publish("/millis", String(millis()/1000).c_str());
    }
    else
      Serial.println("Desconectado");
    refreshServerTime = millis();
  }

  // Corrige o RTC do ESP32
  if ((millis() - corrigeRTC) / 1000 >= T_CORRIGE_ESP32_RTC)
  {
    DateTime now = rtc.now();
    esp32_rtc.setTime(now.unixtime());
    corrigeRTC = millis();
  }
  // if (last_plan != plan)
  // {
  //   current_plan = getTimes(1);
  //   last_plan = plan;
  // }
  // Refresh mqtt subscribes
  client.loop();
  // Debug
  // Serial.printf("WiFi Status: %d | Current menu index: %d | Current level menu index: %d\n", WiFi.status(), current_vertical_num[menu_level], menu_level);
  // Serial.println(millis());
  esp_task_wdt_reset();
  vTaskDelay(10 / portTICK_PERIOD_MS);
}