// #include "defines.h"

// // #ifdef CONFIG_ARDUINO_LOOP_STACK_SIZE
// // #undef CONFIG_ARDUINO_LOOP_STACK_SIZE
// // #define CONFIG_ARDUINO_LOOP_STACK_SIZE 16000
// // #endif

// class SemaphoreBlink
// {
// public:
//   int start_hour = 0;
//   int start_min = 0;
//   int end_hour = 0;
//   int end_min = 0;
//   int start_day = 1;
//   int end_day = 1;
//   bool active = 0;
//   uint32_t start_counter;
//   uint32_t stop_counter;

//   void setStartCounter(uint32_t n)
//   {
//     start_counter = n;
//     start_hour = n / 3600;
//     start_min = n % 3600;
//   }

//   void setEndCounter(uint32_t n)
//   {
//     stop_counter = n;
//     end_hour = n / 3600;
//     end_min = n % 3600;
//   }
//   int check(uint32_t num, uint32_t start, uint32_t end)
//   {
//     if (start < end)
//     {
//       if (num >= start and num <= end)
//         return 1;
//       else
//         return 0;
//     }
//     if (start > end)
//     {
//       if (num < start and num > end)
//         return 0;
//       else
//         return 1;
//     }
//     if (start == end)
//     {
//       if (num == start)
//         return 1;
//       else
//         return 0;
//     }
//     return 0;
//   };
//   uint32_t cvtTimeToSeconds(uint32_t hour, uint32_t min)
//   {
//     return hour * 3600 + min * 60;
//   };
//   // Checa se o horário está dentro da faixa de horarios para o amarelo intermitente
//   int checkTime(ESP32Time *rtc)
//   {
//     // Serial.println("Testando RTC");
//     // Serial.println("---------------------------------------------");
//     uint32_t start = cvtTimeToSeconds(start_hour, start_min);
//     uint32_t end = cvtTimeToSeconds(end_hour, end_min);
//     uint32_t current_time = cvtTimeToSeconds(rtc->getHour(true), rtc->getMinute());
//     uint8_t current_day = rtc->getDayofWeek() + 1; // para 1 porque o domingo foi adotado com numero 1
//     // Serial.printf("Current: %u\tStart: %u\tFinal: %u\n", current_time, start, end);
//     // Serial.printf("Curr. day: %u\tStart: %u\tFinal: %u\n", current_day, start_day, end_day);
//     // Serial.println(check(current_time, start, end));
//     // Serial.println(check(current_day, start_day, end_day));
//     // Serial.println("---------------------------------------------");
//     active = check(current_time, start, end) && check(current_day, start_day, end_day);
//     return active;
//   }
// } blink_semaphore;

// namespace keyboard
// {
//   enum e
//   {
//     PRESSED,
//     RELEASED
//   };
// }
// namespace semaphore
// {
//   enum e
//   {
//     NORMAL,
//     HIGHWAY,
//     BLINK
//   };
// }
// namespace colors
// {
//   enum e
//   {
//     GREEN = 2,
//     YELLOW = 1,
//     RED = 0,
//     OFF
//   };
// }

// // Inicializa o display OLED
// // ADDRESS, SDA, SCL
// // TwoWire I2C1 = TwoWire(0);
// SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_64, I2C_ONE, 100000);

// // ESP32Time rtc;
// ESP32Time esp32_rtc; // offset in seconds GMT+3

// // Objeto referente ao RTC
// RTC_DS1307 rtc;
// String hours_str;

// // Cliente MQTT
// WiFiClient espClient;
// PubSubClient client(espClient);

// WiFiManager wm;

// void top_bar(String msg1, String time, OLEDDISPLAY_TEXT_ALIGNMENT align);
// void body_box(std::vector<String> options, int option);
// void home_box(std::vector<String> options);
// void printCurrentTime(int num);
// void getTimes(int n);
// void savePlan(int n);
// void saveClock(int n);
// void setPlan(int _plan);
// int getPLan();
// void getTimeNTP(int n);
// void cleanWifi(int n);
// void updateClockVars();

// int plan = 1, last_plan = 1;
// int n_channels = 1;
// int semaphore_mode = semaphore::NORMAL; // HIGHWAY; // Define o modo de funcionamento com relação ao semaforo do pedestre
// int time_green = 1, time_yellow = 1, time_red = 1;
// int day = 1, month = 1, year = 2020, hour = 1, minute = 1;
// int current_channel;
// String current_color, pede_current_color;
// int regressive_counter_GREEN = 0;
// int regressive_counter_YELLOW = 0;
// int regressive_counter_RED = 0;

// void Manager::showMenu()
// {
//   if (!sub_menus.empty())
//   {
//     static uint32_t counter;
//     // Obtem o titulo do menu.
//     String title = current_menu()->getTitle();
//     // Obtem strings de cada opção.
//     for (int i = 0; i < current_menu()->subMenusLenght(); i++)
//     {
//       options.push_back(current_menu()->getSubMenuTitle(i));
//     }
//     // int index = increase_option;
//     int index = current_menu()->getOptionIdx();

//     // top_bar(title, hours_str); // Alterar o acesso á esse rtc
//     // Mostra o Menu
//     if (millis() - counter >= REFRESH_DELAY)
//     {
//       // Serial.printf("idle: %d\n",idle);
//       counter = millis();
//       refreshScreen = true;
//     }
//     if (refreshScreen)
//     {
//       refreshScreen = false;
//       display.clear();
//       top_bar(NOME_EMPRESA, "", TEXT_ALIGN_CENTER);
//       if ((millis() - menuIdleCounterTime) / 1000 > IDLE_MENU_LIMIT)
//       {
//         // Menu oscioso
//         idle = true;
//         if (current_color == "VD")
//           regressive_counter_GREEN -= REFRESH_DELAY;
//         if (current_color == "AM")
//           regressive_counter_YELLOW -= REFRESH_DELAY;
//         if (current_color == "VM")
//           regressive_counter_RED -= REFRESH_DELAY;
//         vector<String> lines;
//         String aux = "Estagio: {FASE}      PLN: {PLAN}";
//         aux.replace("{FASE}", String(current_channel));
//         aux.replace("{PLAN}", String(plan));
//         // aux.replace("{TIME}",String(regressive_counter/1000+1));

//         lines.push_back(aux);
//         aux = "Tempo Vermelho: {TIME}";
//         aux.replace("{TIME}", String(regressive_counter_RED/1000));
//         lines.push_back(aux);
//         aux = "Tempo Amarelo: {TIME}";
//         aux.replace("{TIME}", String(regressive_counter_YELLOW/1000));
//         lines.push_back(aux);
//         aux = "Tempo Verde: {TIME}";
//         aux.replace("{TIME}", String(regressive_counter_GREEN/1000));
//         lines.push_back(aux);
//         home_box(lines);
//       }
//       else
//       {
//         // Menu ativo
//         top_bar("Configurações", "", TEXT_ALIGN_CENTER);
//         body_box(options, index);
//       }
//       display.display();
//     }
//     options.clear();
//   }
//   else
//     Serial.println("Outra opção");
// }
// Manager screen_manager = Manager();
// Menu main_menu("Plano {0}", 0, &plan),
//     m1("Plano: {0}", 0, &plan, 1, 6),
//     m2("N° fases: {0}", 0, &n_channels, 1, N_CANAIS_MAX),
//     m3("Pedestres: {0}", 0, &semaphore_mode, 0, 1),
//     m4("Config. Fases"),
//     m5("Ajustar RTC"),
//     m6("NTP Server"),
//     m7("Am. Intermitente"),
//     m8("Salvar"),
//     m9("Desconectar WiFi");

// // Pai: m7
// Menu start_time("Hora de inicio"),
//     end_time("Hora de termino"),
//     select_day("Dia de inicio/fim");

// // Pais: start_time, end_time, select_day
// Menu get_start_hour("Hora: {0}", 0, &blink_semaphore.start_hour, 0, 23),
//     get_start_min("Minuto: {0}", 0, &blink_semaphore.start_min, 0, 59),
//     get_end_hour("Hora: {0}", 0, &blink_semaphore.end_hour, 0, 23),
//     get_end_min("Minuto: {0}", 0, &blink_semaphore.end_min, 0, 59),
//     get_start_day("Inicio: {0}", 0, &blink_semaphore.start_day, 1, 7),
//     get_end_day("Final: {0}", 0, &blink_semaphore.end_day, 1, 7);

// // Pai: m4
// Menu fase1("Fase 1"),
//     fase2("Fase 2"),
//     fase3("Fase 3"),
//     fase4("Fase 4"),
//     fase5("Fase 5"),
//     fase6("Fase 6"),
//     fase7("Fase 7"),
//     fase8("Fase 8");

// // Pai: fase1 ... fase7
// // Menu t_green("Tempo VD: {0}", 0, &tempos, 0, 60, colors::GREEN),
// //     t_yellow("Tempo AM: {0}", 0, &tempos, 0, 60, colors::YELLOW),
// //     t_red("Tempo VM: {0}", 0, &tempos, 0, 60, colors::RED);
// Menu t_green("Tempo VD: {0}"),
//     t_yellow("Tempo AM: {0}"),
//     t_red("Tempo VM: {0}");

// // Pai: m5
// Menu date_day("Dia: {0}", 0, &day, 1, 31),
//     date_month("Mes: {0}", 0, &month, 1, 12),
//     date_year("Ano: {0}", 0, &year, 2020, 2100),
//     date_hour("Horas: {0}", 0, &hour, 0, 23),
//     date_min("Minutos: {0}", 0, &minute, 0, 59);

// Menu _back("Voltar", 1);
// Menu _save_times("Voltar", 1);
// Menu _save_clock("Salvar e Voltar", 1);
// Menu _save_back("Salvar e Voltar", 1, &plan, 0, 6);
// Menu ntp_back("Sincronizar horário", 1);
// Menu clear_wifi_back("Apagar e Voltar", 1);

// void initMenus()
// {
//   // fase1.pushMenu(&_save_times, &saveTimes);
//   fase1.pushMenu(&_save_times);
//   fase1.pushMenu(&t_red);
//   fase1.pushMenu(&t_yellow);
//   fase1.pushMenu(&t_green);

//   fase2.pushMenu(&_save_times, &printCurrentTime);
//   fase2.pushMenu(&t_red);
//   fase2.pushMenu(&t_yellow);
//   fase2.pushMenu(&t_green);

//   fase3.pushMenu(&_save_times);
//   fase3.pushMenu(&t_red);
//   fase3.pushMenu(&t_yellow);
//   fase3.pushMenu(&t_green);

//   fase4.pushMenu(&_save_times);
//   fase4.pushMenu(&t_red);
//   fase4.pushMenu(&t_yellow);
//   fase4.pushMenu(&t_green);

//   fase5.pushMenu(&_save_times);
//   fase5.pushMenu(&t_red);
//   fase5.pushMenu(&t_yellow);
//   fase5.pushMenu(&t_green);

//   fase6.pushMenu(&_save_times);
//   fase6.pushMenu(&t_red);
//   fase6.pushMenu(&t_yellow);
//   fase6.pushMenu(&t_green);

//   fase7.pushMenu(&_save_times);
//   fase7.pushMenu(&t_red);
//   fase7.pushMenu(&t_yellow);
//   fase7.pushMenu(&t_green);

//   fase8.pushMenu(&_save_times);
//   fase8.pushMenu(&t_red);
//   fase8.pushMenu(&t_yellow);
//   fase8.pushMenu(&t_green);

//   m4.pushMenu(&_back);
//   m4.pushMenu(&fase1);
//   m4.pushMenu(&fase2);
//   m4.pushMenu(&fase3);
//   m4.pushMenu(&fase4);
//   m4.pushMenu(&fase5);
//   m4.pushMenu(&fase6);
//   m4.pushMenu(&fase7);
//   m4.pushMenu(&fase8);

//   m5.pushMenu(&_back);
//   m5.pushMenu(&_save_clock, &saveClock);
//   m5.pushMenu(&date_day);
//   m5.pushMenu(&date_month);
//   m5.pushMenu(&date_year);
//   m5.pushMenu(&date_hour);
//   m5.pushMenu(&date_min);

//   m6.pushMenu(&_back);
//   m6.pushMenu(&ntp_back, &getTimeNTP);

//   start_time.pushMenu(&_back);
//   start_time.pushMenu(&get_start_hour);
//   start_time.pushMenu(&get_start_min);

//   end_time.pushMenu(&_back);
//   end_time.pushMenu(&get_end_hour);
//   end_time.pushMenu(&get_end_min);

//   select_day.pushMenu(&_back);
//   select_day.pushMenu(&get_start_day);
//   select_day.pushMenu(&get_end_day);

//   m7.pushMenu(&_back);
//   m7.pushMenu(&start_time);
//   m7.pushMenu(&end_time);
//   m7.pushMenu(&select_day);

//   m8.pushMenu(&_back);
//   m8.pushMenu(&_save_back, &savePlan);

//   m9.pushMenu(&_back);
//   m9.pushMenu(&clear_wifi_back, &cleanWifi);

//   main_menu.pushMenu(&m1);
//   main_menu.pushMenu(&m2);
//   main_menu.pushMenu(&m3);
//   main_menu.pushMenu(&m4);
//   main_menu.pushMenu(&m5);
//   main_menu.pushMenu(&m6);
//   main_menu.pushMenu(&m7);
//   main_menu.pushMenu(&m8);
//   main_menu.pushMenu(&m9);

//   screen_manager.pushMenu(&main_menu);
// }

// PCF8574 canal1(0x27, SDA, SCL);
// PCF8574 canal2(0x23, SDA, SCL);
// PCF8574 canal3(0x21, SDA, SCL);
// PCF8574 canal4(0x20, SDA, SCL);

// void setFase(int fase, int color, int value)
// {
//   if (!(fase % 2))
//     color += 3;
//   switch (fase)
//   {
//   case 1:
//     canal1.digitalWrite(color, value);
//     break;
//   case 2:
//     canal1.digitalWrite(color, value);
//     break;
//   case 3:
//     canal2.digitalWrite(color, value);
//     break;
//   case 4:
//     canal2.digitalWrite(color, value);
//     break;
//   case 5:
//     canal3.digitalWrite(color, value);
//     break;
//   case 6:
//     canal3.digitalWrite(color, value);
//     break;
//   case 7:
//     canal4.digitalWrite(color, value);
//     break;
//   case 8:
//     canal4.digitalWrite(color, value);
//     break;
//   }
// }

// uint8_t bt_up = keyboard::RELEASED;
// uint8_t bt_down = keyboard::RELEASED;
// uint8_t bt_left = keyboard::RELEASED;
// uint8_t bt_right = keyboard::RELEASED;
// uint8_t bt_center = keyboard::RELEASED;
// uint8_t bt_pede = keyboard::RELEASED;
// bool key_clicked = false;
// bool pede_clicked = false;
// unsigned long refreshServerTime = 5;
// unsigned long corrigeRTC = 0;
// const char *serverName = "https://semapho.herokuapp.com/api2";

// void top_bar(String msg1, String time, OLEDDISPLAY_TEXT_ALIGNMENT align)
// {
//   int offset = 0;
//   if (align == TEXT_ALIGN_CENTER)
//     offset = 128 / 2;
//   display.setFont(ArialMT_Plain_10);
//   display.fillRect(0, 0, 128, 15);
//   display.setColor(BLACK);
//   display.setTextAlignment(align);
//   display.drawString(offset + 1, 0, msg1);
//   // display.setTextAlignment(TEXT_ALIGN_CENTER);
//   // if(WiFi.status() == WL_CONNECTED){
//   //   time = time + "/ON";
//   // }
//   // else{
//   //   time = time + "/OFF";
//   // }
//   display.setTextAlignment(TEXT_ALIGN_RIGHT);
//   display.drawString(127, 0, time);
//   display.setColor(WHITE);
// }

// void body_box(std::vector<String> options, int option)
// {
//   int first_option = (option > 3) ? option - 3 : 0;
//   int nlines = min(int(options.size()), 4);
//   display.setColor(WHITE);
//   display.setFont(ArialMT_Plain_10);
//   display.setTextAlignment(TEXT_ALIGN_LEFT);
//   for (int i = first_option, j = 16; i < first_option + nlines; i++, j += 10)
//   {
//     if (i == option)
//       options[i] = ">" + options[i];
//     else
//       options[i] = " " + options[i];
//     display.drawString(1, j, options[i]);
//   }
//   // display.drawRect(0, 16, 128, 48);
// }

// void home_box(std::vector<String> options)
// {
//   display.setColor(WHITE);
//   display.setFont(ArialMT_Plain_10);
//   display.setTextAlignment(TEXT_ALIGN_LEFT);
//   for (int i = 0, j = 16; i < options.size(); i++, j += 10)
//   {
//     display.drawString(1, j, options[i]);
//   }
//   // display.drawRect(0, 16, 128, 48);
//   updateClockVars();
// }

// void beginScreen(String title)
// {
//   uint8_t width = 128;
//   display.setFont(ArialMT_Plain_10);
//   display.fillRect(0, 0, width, 15);
//   display.setColor(BLACK);
//   display.setTextAlignment(TEXT_ALIGN_CENTER);
//   display.drawString(width / 2, 0, title);
//   display.setColor(WHITE);
//   display.drawString(width / 2, 20, "Semaforo Iniciando");
//   display.drawString(width / 2, 30, "Aguarde...");
//   display.setColor(WHITE);
// }

// String getId(String prefix, int n)
// {
//   char id[23];
//   int begin = 9;

//   if (n < 13 && n > 0)
//     begin += n;
//   else
//     begin = 23;

//   snprintf(id, begin, "%s%llX", prefix.c_str(), ESP.getEfuseMac());
//   return String(id);
// }
// /****************************************************************/
// bool initRTC()
// {
//   if (!rtc.begin(&Wire))
//   {
//     Serial.println("Couldn't find RTC");
//     return false;
//   }
//   if (!rtc.isrunning())
//   {
//     Serial.println("RTC is NOT running, let's set the time!");
//     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//     // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
//     return false;
//   }
//   return true;
// }
// // Inicialização das saidas para leds
// // São utilizados apenas 6 bits de cada chip
// void initOutputs()
// {
//   for (int i = 0; i < 6; i++)
//   {
//     canal1.pinMode(i, OUTPUT, 0);
//   }
//   for (int i = 0; i < 6; i++)
//   {
//     canal2.pinMode(i, OUTPUT, 0);
//   }
//   for (int i = 0; i < 6; i++)
//   {
//     canal3.pinMode(i, OUTPUT, 0);
//   }
//   for (int i = 0; i < 6; i++)
//   {
//     canal4.pinMode(i, OUTPUT, 0);
//   }
//   canal1.begin();
//   canal2.begin();
//   canal3.begin();
//   canal4.begin();
// }
// // Inicialização das entradas digitais necessarias
// void initInputs()
// {
//   pinMode(BT_UP, INPUT_PULLUP);
//   pinMode(BT_DOWN, INPUT_PULLUP);
//   pinMode(BT_LEFT, INPUT_PULLUP);
//   pinMode(BT_RIGHT, INPUT_PULLUP);
//   pinMode(BT_CENTER, INPUT_PULLUP);
//   pinMode(BT_PEDE, INPUT_PULLUP);
// }
// // Pooling para os botões
// void switchPolling(void *parameter)
// {
//   pinMode(2, OUTPUT);
//   esp_task_wdt_add(NULL);
//   for (;;)
//   {
//     digitalWrite(2,
//                  !digitalRead(BT_UP) ||
//                      !digitalRead(BT_DOWN) ||
//                      !digitalRead(BT_LEFT) ||
//                      !digitalRead(BT_RIGHT) ||
//                      !digitalRead(BT_CENTER) ||
//                      !digitalRead(BT_PEDE));
//     if (!digitalRead(BT_UP))
//     {
//       vTaskDelay(250 / portTICK_PERIOD_MS);
//       bt_up = keyboard::PRESSED;
//     }
//     else
//     {
//       if (bt_up == keyboard::PRESSED)
//       {
//         // retorna uma posição no menu.
//         screen_manager.goBackOption();
//         bt_up = keyboard::RELEASED;
//         key_clicked = true;
//       }
//     }
//     if (!digitalRead(BT_DOWN))
//     {
//       vTaskDelay(50 / portTICK_PERIOD_MS);
//       bt_down = keyboard::PRESSED;
//     }
//     else
//     {
//       if (bt_down == keyboard::PRESSED)
//       {
//         // Avança para a proxima opção no menu.
//         screen_manager.nextOption();
//         bt_down = keyboard::RELEASED;
//         key_clicked = true;
//       }
//     }
//     if (!digitalRead(BT_LEFT))
//     {
//       vTaskDelay(50 / portTICK_PERIOD_MS);
//       bt_left = keyboard::PRESSED;
//     }
//     else
//     {
//       if (bt_left == keyboard::PRESSED)
//       {
//         // Incrementa uma variavel que o menu controla
//         screen_manager.decreaseOption();
//         bt_left = keyboard::RELEASED;
//         key_clicked = true;
//       }
//     }
//     if (!digitalRead(BT_RIGHT))
//     {
//       vTaskDelay(50 / portTICK_PERIOD_MS);
//       bt_right = keyboard::PRESSED;
//     }
//     else
//     {
//       if (bt_right == keyboard::PRESSED)
//       {
//         // Incrementa uma variavel que o menu controla
//         screen_manager.increaseOption();
//         bt_right = keyboard::RELEASED;
//         key_clicked = true;
//       }
//     }
//     if (!digitalRead(BT_CENTER))
//     {
//       vTaskDelay(50 / portTICK_PERIOD_MS);
//       bt_center = keyboard::PRESSED;
//     }
//     else
//     {
//       if (bt_center == keyboard::PRESSED)
//       {
//         key_clicked = true;
//         bt_center = keyboard::RELEASED;
//         screen_manager.nextSubMenu();
//       }
//     }
//     if (!digitalRead(BT_PEDE))
//     {
//       vTaskDelay(50 / portTICK_PERIOD_MS);
//       bt_pede = keyboard::PRESSED;
//     }
//     else
//     {
//       if (bt_pede == keyboard::PRESSED)
//       {
//         bt_pede = keyboard::RELEASED;
//         pede_clicked = true;
//       }
//     }
//     esp_task_wdt_reset();
//     vTaskDelay(50 / portTICK_PERIOD_MS);
//   }
// }

// void ledsLoop(void *parameter)
// {
//   esp_task_wdt_add(NULL);
//   getTimes(0);
//   Serial.printf("Plano atual:%d\n", plan);
//   // Amarelo intermitente inicial
//   for (int i = 0; i < 3; i++)
//   {
//     n_channels = 4;
//     for (int i = 1; i <= n_channels; i++)
//     {
//       esp_task_wdt_reset();
//       setFase(i, colors::YELLOW, 1);
//     }
//     vTaskDelay(1000 / portTICK_PERIOD_MS); //
//     for (int i = 1; i <= 7; i++)
//     {
//       esp_task_wdt_reset();
//       setFase(i, colors::YELLOW, 0);
//     }
//     vTaskDelay(1000 / portTICK_PERIOD_MS); //
//   }
//   // Vermelhos 3 segundos
//   for (int i = 1; i <= n_channels; i++)
//   {
//     esp_task_wdt_reset();
//     setFase(i, colors::RED, 1);
//   }
//   vTaskDelay(3000 / portTICK_PERIOD_MS); //
//                                          // for (int i = 1; i <= 7; i++)
//                                          // {
//                                          //   esp_task_wdt_reset();
//                                          //   setFase(i, colors::YELLOW, 0);
//                                          // }
//                                          // vTaskDelay(1000 / portTICK_PERIOD_MS); //
//   for (;;)
//   {
//     esp_task_wdt_reset();
//     // Modo via comum
//     // Inicializando semaforos
//     for (int i = 1; i <= n_channels; i++)
//     {
//       esp_task_wdt_reset();
//       setFase(i, colors::GREEN, 0);
//       setFase(i, colors::YELLOW, 0);
//       setFase(i, colors::RED, 1);
//     }

//     while (semaphore_mode == semaphore::NORMAL)
//     {
//       esp_task_wdt_reset();
//       int canal_counter = 0;
//       bool canal_turn_off = false;
//       // Checa o numero de canals ajustado pelo usuario
//       // if(canal_counter >= n_channels)
//       // {
//       //   canal_turn_off = true;
//       // }
//       // canal_counter ++;
//       for (int i = 1; i <= n_channels; i++)
//       {
//         current_channel = i;
//         if (pede_clicked)
//         {
//           pede_current_color = "VD";
//           setFase(FASE_PEDE, colors::GREEN, 1);
//           setFase(FASE_PEDE, colors::RED, 0);
//           vTaskDelay(5000 / portTICK_PERIOD_MS);
//           pede_current_color = "VM";
//           setFase(FASE_PEDE, colors::GREEN, 0);
//           setFase(FASE_PEDE, colors::RED, 1);
//           vTaskDelay(100 / portTICK_PERIOD_MS);
//           pede_clicked = false;
//         }
//         else
//         {
//           pede_current_color = "VM";
//           setFase(FASE_PEDE, colors::GREEN, 0);
//           setFase(FASE_PEDE, colors::YELLOW, 0);
//           setFase(FASE_PEDE, colors::RED, 1);
//         }

//         regressive_counter_GREEN = screen_manager.tempos[i - 1][colors::GREEN] * 1000;
//         regressive_counter_YELLOW = screen_manager.tempos[i - 1][colors::YELLOW] * 1000;
//         regressive_counter_RED = screen_manager.tempos[i - 1][colors::RED] * 1000;
//         // Semaforo verde
//         current_color = "VD";        
//         setFase(i, colors::GREEN, 1);
//         setFase(i, colors::YELLOW, 0);
//         setFase(i, colors::RED, 0);
//         vTaskDelay(screen_manager.tempos[i - 1][colors::GREEN] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos

//         // Semaforo amarelo
//         current_color = "AM";
//         setFase(i, colors::GREEN, 0);
//         setFase(i, colors::YELLOW, 1);
//         setFase(i, colors::RED, 0);
//         vTaskDelay(screen_manager.tempos[i - 1][colors::YELLOW] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos

//         // Semaforo vermelho
//         current_color = "VM";        
//         setFase(i, colors::GREEN, 0);
//         setFase(i, colors::YELLOW, 0);
//         setFase(i, colors::RED, 1);
//         vTaskDelay(screen_manager.tempos[i - 1][colors::RED] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
//       }
//       for (int i = 8; i > n_channels; i--)
//       {
//         setFase(i, colors::GREEN, 0);
//         setFase(i, colors::YELLOW, 0);
//         setFase(i, colors::RED, 0);
//       }
//     }
//     for (int i = 1; i <= n_channels; i++)
//     {
//       esp_task_wdt_reset();
//       setFase(i, colors::GREEN, 0);
//       setFase(i, colors::YELLOW, 0);
//       setFase(i, colors::RED, 0);
//     }
//     // Amarelo intermitente
//     while (semaphore_mode == semaphore::HIGHWAY)
//     {
//       esp_task_wdt_reset();
//       current_channel = 0;
//       // Verde do pedestre aceso
//       setFase(FASE_PEDE, colors::GREEN, 0);
//       setFase(FASE_PEDE, colors::YELLOW, 0);
//       setFase(FASE_PEDE, colors::RED, 1);
//       // Primeira fase semaforo dos Carros
//       current_color = "VD";
//       setFase(1, colors::GREEN, 1);
//       setFase(1, colors::YELLOW, 0);
//       setFase(1, colors::RED, 0);

//       if (pede_clicked)
//       {
//         // Acende amarelo
//         esp_task_wdt_reset();
//         current_color = "AM";
//         vTaskDelay(screen_manager.tempos[0][colors::GREEN] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
//         setFase(1, colors::GREEN, 0);
//         setFase(1, colors::YELLOW, 1);
//         setFase(1, colors::RED, 0);

//         // Acende vermelho
//         esp_task_wdt_reset();
//         vTaskDelay(screen_manager.tempos[0][colors::YELLOW] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
//         setFase(1, colors::GREEN, 0);
//         setFase(1, colors::YELLOW, 0);
//         setFase(1, colors::RED, 1);
//         vTaskDelay(screen_manager.tempos[0][colors::RED] * 1000 / portTICK_PERIOD_MS); // x1000 converte para segundos
//         // Verde dos pedestres
//         esp_task_wdt_reset();
//         pede_current_color = "VD";
//         setFase(FASE_PEDE, colors::GREEN, 1);
//         setFase(FASE_PEDE, colors::YELLOW, 0);
//         setFase(FASE_PEDE, colors::RED, 0);
//         vTaskDelay(screen_manager.tempos[7][colors::GREEN] * 1000 / portTICK_PERIOD_MS);

//         esp_task_wdt_reset();
//         pede_current_color = "VM";
//         setFase(FASE_PEDE, colors::GREEN, 0);
//         setFase(FASE_PEDE, colors::YELLOW, 0);
//         setFase(FASE_PEDE, colors::RED, 1);

//         // Pisca o vermelho dos pedestres
//         esp_task_wdt_reset();
//         for (int i = 0; i <= screen_manager.tempos[7][colors::RED]; i++)
//         {
//           vTaskDelay(500 / portTICK_PERIOD_MS);
//           setFase(FASE_PEDE, colors::RED, 1);
//           vTaskDelay(500 / portTICK_PERIOD_MS);
//           setFase(FASE_PEDE, colors::RED, 0);
//         }
//         pede_clicked = false;
//       }
//     }
//     for (int i = 1; i <= n_channels; i++)
//     {
//       esp_task_wdt_reset();
//       setFase(i, colors::GREEN, 0);
//       setFase(i, colors::YELLOW, 1);
//       setFase(i, colors::RED, 0);
//     }
//     while (semaphore_mode == semaphore::BLINK)
//     {
//       current_color = "AM";
//       current_channel = 0;
//       for (int i = 1; i <= n_channels; i++)
//       {
//         esp_task_wdt_reset();
//         setFase(i, colors::YELLOW, 1);
//       }
//       vTaskDelay(1000 / portTICK_PERIOD_MS); //
//       for (int i = 1; i <= 7; i++)
//       {
//         esp_task_wdt_reset();
//         setFase(i, colors::YELLOW, 0);
//       }
//       vTaskDelay(1000 / portTICK_PERIOD_MS); //
//     }
//   }
// }

// // Guarda as configurações de tempo de todas as fases
// void savePlan(int n)
// {
//   Preferences p;

//   String name = "plan" + String(plan);

//   Serial.printf("Salvando plano: %s\n", name.c_str());

//   setPlan(plan);

//   p.begin(name.c_str(), false);

//   // Salva os dias e horarios do amarelo intermitente
//   String key = "BEGIN_DAY";
//   p.putInt(key.c_str(), blink_semaphore.start_day);
//   key = "END_DAY";
//   p.putInt(key.c_str(), blink_semaphore.end_day);
//   key = "BEGIN_H";
//   p.putInt(key.c_str(), blink_semaphore.start_hour);
//   key = "END_HOUR";
//   p.putInt(key.c_str(), blink_semaphore.end_hour);
//   key = "BEGIN_MIN";
//   p.putInt(key.c_str(), blink_semaphore.start_min);
//   key = "END_MIN";
//   p.putInt(key.c_str(), blink_semaphore.end_min);

//   // Salva o tipo de pedestre do plano atual
//   key = "WALKER_TYPE";
//   if (semaphore_mode < 2)
//     p.putInt(key.c_str(), semaphore_mode);

//   // Salva a quantidade de canais do plano atual
//   key = "N_CHANNELS";
//   p.putInt(key.c_str(), n_channels);

//   for (int i = 0; i < 8; i++)
//   {
//     key = "F{}_GREEN";
//     key.replace("{}", String(i));
//     p.putInt(key.c_str(), screen_manager.tempos[i][colors::GREEN]);
//     Serial.printf("Verde: %s: %d\t ", key.c_str(), p.getInt(key.c_str(), 1));

//     key = "F{}_YELLOW";
//     key.replace("{}", String(i));
//     p.putInt(key.c_str(), screen_manager.tempos[i][colors::YELLOW]);
//     Serial.printf("Amarelo %s: %d\t", key.c_str(), p.getInt(key.c_str(), 1));

//     key = "F{}_RED";
//     key.replace("{}", String(i));
//     p.putInt(key.c_str(), screen_manager.tempos[i][colors::RED]);
//     Serial.printf("Vermelho: %s: %d\n", key.c_str(), p.getInt(key.c_str(), 1));
//   }
//   p.end();
// }

// void getTimes(int n = 0)
// {
//   // Obtendo o plano
//   if (n <= 0)
//     plan = getPLan();

//   String name = "plan" + String(plan);
//   Serial.printf("Plano: %s\n", name.c_str());

//   Preferences p;
//   p.begin(name.c_str(), false);

//   // Tipo de pedestre
//   String key = "WALKER_TYPE";
//   semaphore_mode = p.getInt(key.c_str(), 0);

//   key = "BEGIN_DAY";
//   blink_semaphore.start_day = p.getInt(key.c_str(), 1);
//   key = "END_DAY";
//   blink_semaphore.end_day = p.getInt(key.c_str(), 1);
//   key = "BEGIN_HOUR";
//   blink_semaphore.start_hour = p.getInt(key.c_str(), 0);
//   key = "END_HOUR";
//   blink_semaphore.end_hour = p.getInt(key.c_str(), 0);
//   key = "BEGIN_MIN";
//   blink_semaphore.start_min = p.getInt(key.c_str(), 0);
//   key = "END_MIN";
//   blink_semaphore.end_min = p.getInt(key.c_str(), 0);

//   // Numero de canais
//   key = "N_CHANNELS";
//   n_channels = p.getInt(key.c_str(), 1);

//   for (int i = 0; i < 8; i++)
//   {
//     String key = "F{}_GREEN";
//     key.replace("{}", String(i));
//     screen_manager.tempos[i][colors::GREEN] = p.getInt(key.c_str(), 0);
//     key = "F{}_YELLOW";
//     key.replace("{}", String(i));
//     screen_manager.tempos[i][colors::YELLOW] = p.getInt(key.c_str(), 0);
//     key = "F{}_RED";
//     key.replace("{}", String(i));
//     screen_manager.tempos[i][colors::RED] = p.getInt(key.c_str(), 0);
//     Serial.printf("tempos[%d] = [%d, %d, %d]\n", i, screen_manager.tempos[i][colors::GREEN], screen_manager.tempos[i][colors::YELLOW], screen_manager.tempos[i][colors::RED]);
//   }
//   p.end();
// }

// void saveClock(int n)
// {
//   // Ajuste do dia de acordo com o mês e se o ano é bisexto
//   if (month == 4 || month == 6 || month == 9 || month == 11)
//     day = min(day, 30);
//   if (month == 2 && !(year % 4))
//   {
//     if ((year % 100))
//       day = min(day, 29);
//     else
//       day = min(day, 28);
//   }
//   // Ajuste dos RTCs
//   rtc.adjust(DateTime(year, month, day, hour, minute, 0));
//   DateTime now = rtc.now();
//   esp32_rtc.setTime(0, minute, hour, day, month, year);
// }

// void printCurrentTime(int num = 0)
// {
//   DateTime now = rtc.now();
//   char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//   Serial.print(now.year(), DEC);
//   Serial.print('/');
//   Serial.print(now.month(), DEC);
//   Serial.print('/');
//   Serial.print(now.day(), DEC);
//   Serial.print(" (");
//   Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
//   Serial.print(") ");
//   Serial.print(now.hour(), DEC);
//   Serial.print(':');
//   Serial.print(now.minute(), DEC);
//   Serial.print(':');
//   Serial.print(now.second(), DEC);
//   Serial.println();
// }

// void setPlan(int _plan = 1)
// {
//   Preferences p;
//   p.begin("CONFIGS", false);
//   p.putInt("CURRENT_PLAN", _plan);
//   p.end();
// }

// int getPLan()
// {
//   Preferences p;
//   int _plan = 1;
//   p.begin("CONFIGS", false);
//   _plan = p.getInt("CURRENT_PLAN", 1);
//   p.end();
//   return _plan;
// }

// void wifiSetup(void *parameter)
// {
//   // esp_task_wdt_add(NULL);
//   // esp_task_wdt_reset();
//   // wm.setClass("invert"); // dark theme
//   wm.setScanDispPerc(true); // display percentages instead of graphs for RSSI
//   // wm.resetSettings();
//   bool res = wm.autoConnect(); // password protected ap
//   // bool res = wm.startConfigPortal();
//   if (!res)
//   {
//     Serial.println("Failed to connect");
//     // ESP.restart();
//   }
//   else
//   {
//     // if you get here you have connected to the WiFi
//     Serial.println("connected...");
//   }
//   Serial.println("Encerrando Task");
//   vTaskDelete(NULL);
// }

// void cleanWifi(int n)
// {
//   wm.resetSettings();
// }

// void callback(char *topic, byte *message, unsigned int length)
// {
//   Serial.print("Message arrived on topic: ");
//   Serial.print(topic);
//   Serial.print(". Message: ");
//   String messageTemp;

//   for (int i = 0; i < length; i++)
//   {
//     Serial.print((char)message[i]);
//     messageTemp += (char)message[i];
//   }
//   Serial.println();

//   if (!strcmp(topic, "n_fases"))
//   {
//     n_channels = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "n_plano"))
//   {
//     plan = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "d_inicio"))
//   {
//     blink_semaphore.start_day = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "d_fim"))
//   {
//     blink_semaphore.end_day = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "h_inicio"))
//   {
//     blink_semaphore.setStartCounter(messageTemp.toInt());
//   }
//   if (!strcmp(topic, "h_fim"))
//   {
//     blink_semaphore.setEndCounter(messageTemp.toInt());
//   }

//   if (!strcmp(topic, "vd_t1"))
//   {
//     screen_manager.tempos[0][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t1"))
//   {
//     screen_manager.tempos[0][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t1"))
//   {
//     screen_manager.tempos[0][colors::RED] = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "vd_t2"))
//   {
//     screen_manager.tempos[1][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t2"))
//   {
//     screen_manager.tempos[1][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t2"))
//   {
//     screen_manager.tempos[1][colors::RED] = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "vd_t3"))
//   {
//     screen_manager.tempos[2][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t3"))
//   {
//     screen_manager.tempos[2][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t3"))
//   {
//     screen_manager.tempos[2][colors::RED] = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "vd_t4"))
//   {
//     screen_manager.tempos[3][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t4"))
//   {
//     screen_manager.tempos[3][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t4"))
//   {
//     screen_manager.tempos[3][colors::RED] = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "vd_t5"))
//   {
//     screen_manager.tempos[4][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t5"))
//   {
//     screen_manager.tempos[4][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t5"))
//   {
//     screen_manager.tempos[4][colors::RED] = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "vd_t6"))
//   {
//     screen_manager.tempos[5][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t6"))
//   {
//     screen_manager.tempos[5][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t6"))
//   {
//     screen_manager.tempos[5][colors::RED] = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "vd_t7"))
//   {
//     screen_manager.tempos[6][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t7"))
//   {
//     screen_manager.tempos[6][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t7"))
//   {
//     screen_manager.tempos[6][colors::RED] = messageTemp.toInt();
//   }

//   if (!strcmp(topic, "vd_t8"))
//   {
//     screen_manager.tempos[7][colors::GREEN] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "am_t8"))
//   {
//     screen_manager.tempos[7][colors::YELLOW] = messageTemp.toInt();
//   }
//   if (!strcmp(topic, "vm_t8"))
//   {
//     screen_manager.tempos[7][colors::RED] = messageTemp.toInt();
//   }
//   screen_manager.refreshScreen = true;
// }

// void subAllTopics()
// {
//   client.subscribe("n_plano", 1);
//   client.subscribe("n_fases", 1);
//   client.subscribe("d_inicio", 1);
//   client.subscribe("d_fim", 1);
//   client.subscribe("h_inicio", 1);
//   client.subscribe("h_fim", 1);
//   client.subscribe("vd_t1", 1);
//   client.subscribe("vd_t2", 1);
//   client.subscribe("vd_t3", 1);
//   client.subscribe("vd_t4", 1);
//   client.subscribe("vd_t5", 1);
//   client.subscribe("vd_t6", 1);
//   client.subscribe("vd_t7", 1);
//   client.subscribe("vd_t8", 1);

//   client.subscribe("am_t1", 1);
//   client.subscribe("am_t2", 1);
//   client.subscribe("am_t3", 1);
//   client.subscribe("am_t4", 1);
//   client.subscribe("am_t5", 1);
//   client.subscribe("am_t6", 1);
//   client.subscribe("am_t7", 1);
//   client.subscribe("am_t8", 1);

//   client.subscribe("vm_t1", 1);
//   client.subscribe("vm_t2", 1);
//   client.subscribe("vm_t3", 1);
//   client.subscribe("vm_t4", 1);
//   client.subscribe("vm_t5", 1);
//   client.subscribe("vm_t6", 1);
//   client.subscribe("vm_t7", 1);
//   client.subscribe("vm_t8", 1);
// }

// void mqttSetup()
// {
//   client.setServer(SERVER, PORT);
//   client.setCallback(callback);
//   // client.publish(PUB_TOPIC, "teste");
// }

// void mqttReconnect()
// {
//   // Loop until we're reconnected
//   while (!client.connected())
//   {
//     Serial.print("Attempting MQTT connection...");
//     // Attempt to connect
//     if (client.connect("ifgunxgo", USER, PASS))
//     {
//       Serial.println("connected");
//       // Subscribe
//       subAllTopics();
//     }
//     else
//     {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       // Wait 5 seconds before retrying
//       delay(5000);
//     }
//   }
// }

// void getTimeNTP(int n)
// {
//   if (WiFi.status() != WL_CONNECTED)
//     return;
//   WiFiUDP udp;
//   NTPClient ntp(udp, "a.st1.ntp.br", -3 * 3600, 60000); /* Cria um objeto "NTP" com as configurações.utilizada no Brasil */
//   ntp.begin();                                          /* Inicia o protocolo */
//   ntp.forceUpdate();                                    /* Atualização */
//   String formattedDate = ntp.getFormattedDate();

//   // Extract date
//   int split_T = formattedDate.indexOf("T");
//   int split_ = formattedDate.indexOf("-");
//   String dayStamp = formattedDate.substring(0, split_T);
//   String day, month, year;
//   year = dayStamp.substring(0, split_);
//   dayStamp.replace(year + "-", "");
//   split_ = dayStamp.indexOf("-");

//   month = dayStamp.substring(0, split_);
//   dayStamp.replace(month + "-", "");
//   split_ = dayStamp.indexOf("-");

//   day = dayStamp.substring(0, split_);

//   // Serial.printf("\n%d %d %d\n", day.toInt(), month.toInt(), year.toInt());

//   // Extract time
//   String timeStamp = formattedDate.substring(split_T + 1, formattedDate.length() - 1);

//   int split_dp = timeStamp.indexOf(":");
//   String hour, _min, sec;

//   hour = timeStamp.substring(0, split_dp);
//   timeStamp.replace(hour + ":", "");
//   split_dp = timeStamp.indexOf(":");

//   _min = timeStamp.substring(0, split_dp);
//   timeStamp.replace(_min + ":", "");
//   split_dp = timeStamp.indexOf(":");

//   sec = timeStamp.substring(0, split_dp);
//   // Serial.printf("\n%d %d %d\n", hour.toInt(), _min.toInt(), sec.toInt());
//   rtc.adjust(DateTime(year.toInt(), month.toInt(), day.toInt(), hour.toInt(), _min.toInt(), sec.toInt()));
//   DateTime now = rtc.now();
//   esp32_rtc.setTime(now.unixtime());
//   Serial.println("Correção de horario via NTP");
// }

// void updateClockVars()
// {
//   minute = esp32_rtc.getMinute();
//   hour = esp32_rtc.getHour(true);
//   day = esp32_rtc.getDay();
//   month = esp32_rtc.getMonth();
//   year = esp32_rtc.getYear();
// }

// void setup()
// {
//   Serial.begin(115200);
//   // Configura o wdt em 60s
//   int ret = esp_task_wdt_init(60, true);
//   if (ret == ESP_OK)
//     Serial.println("TWDT successfully deinitialized");
//   if (ret == ESP_ERR_INVALID_STATE)
//     Serial.println("Error, tasks are still subscribed to the TWDT");
//   if (ret == ESP_ERR_NOT_FOUND)
//     Serial.println("Error, TWDT has already been deinitialized");
//   esp_task_wdt_add(NULL);
//   Serial.println(CONFIG_ARDUINO_LOOP_STACK_SIZE);
//   Serial.println();

//   // I2C1.begin(SDA, SCL, uint32_t(100000));
//   Wire.begin(SDA, SCL, uint32_t(100000));
//   initOutputs();
//   initInputs();
//   initMenus();
//   initRTC();
//   DateTime now = rtc.now();
//   esp32_rtc.setTime(now.unixtime());

//   xTaskCreate(switchPolling, "switch_polling", 14048, NULL, 1, NULL);
//   xTaskCreate(ledsLoop, "leds_loop", 4048, NULL, 1, NULL);

//   display.init();
//   display.flipScreenVertically();
//   // display.drawXbm(14, 14, IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_NAME);
//   // display.display();
//   printCurrentTime();
//   vTaskDelay(2000 / portTICK_PERIOD_MS);
//   beginScreen(NOME_EMPRESA);
//   display.display();
//   vTaskDelay(4000 / portTICK_PERIOD_MS);

//   xTaskCreate(wifiSetup, "wifi_setup", 4048, NULL, 10, NULL);
//   mqttSetup();
// }

// void loop()
// {
//   hours_str = esp32_rtc.getTime("%H:%M");
//   screen_manager.showMenu();
//   // Serial.println(semaphore_mode);
//   if (key_clicked)
//   {
//     key_clicked = false;
//   }
//   // Teste de requisição em 10s
//   if ((millis() - refreshServerTime) / 1000 >= 10)
//   {
//     // Serial.println("Request server");
//     // requestServer(serverName);
//     // Serial.printf("Refresh server\n");
//     if (WiFi.status() == WL_CONNECTED)
//     {
//       mqttReconnect();
//       // Serial.printf("MQTT publish: %d\n", client.publish("/millis", String(millis()/1000).c_str()));
//       // client.publish("/millis", String(millis()/1000).c_str());
//     }
//     else
//       Serial.println("Desconectado");
//     refreshServerTime = millis();
//   }

//   // Corrige o RTC do ESP32
//   if ((millis() - corrigeRTC) / 1000 >= T_CORRIGE_ESP32_RTC)
//   {
//     DateTime now = rtc.now();
//     esp32_rtc.setTime(now.unixtime());
//     corrigeRTC = millis();
//   }
//   if (last_plan != plan)
//   {
//     getTimes(1);
//     last_plan = plan;
//   }
//   // Checa se horario esta configurado como amarelo intermitente
//   if (blink_semaphore.checkTime(&esp32_rtc))
//   {
//     // printCurrentTime();
//     semaphore_mode = semaphore::BLINK;
//   }
//   if (semaphore_mode == semaphore::BLINK && !blink_semaphore.checkTime(&esp32_rtc))
//   {
//     Serial.println("DEBUG");
//     // volta ao estado salvo no plano
//     Preferences p;
//     String name = "plan" + String(plan);
//     p.begin(name.c_str(), false);
//     String key = "WALKER_TYPE";
//     semaphore_mode = p.getInt(key.c_str(), 0);
//     Serial.printf("semaphore_mode: %d\n", semaphore_mode);
//     p.end();
//   }
//   // Refresh mqtt subscribes
//   client.loop();
//   // Debug
//   // Serial.printf("WiFi Status: %d | Current menu index: %d | Current level menu index: %d\n", WiFi.status(), current_vertical_num[menu_level], menu_level);
//   // Serial.println(millis());
//   esp_task_wdt_reset();
//   vTaskDelay(10 / portTICK_PERIOD_MS);
// }