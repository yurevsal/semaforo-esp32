#include <plan.h>

// Plain_model::Plain_model(uint8_t _id)
// {
//     Serial.printf("[PLAIN_MODEL]: Init instance\n");
//     id = _id;
//     load();
// }

// void Plain_model::setStartTime(uint32_t time)
// {
//     start = time;
// }
// void Plain_model::setPhasesActives(uint32_t n)
// {
//     n_phases = n;
// }
// void Plain_model::setLightsTimes(uint8_t idx, uint16_t g, uint16_t y, uint16_t r)
// {
//     phases[idx].t_green = g;
//     phases[idx].t_yellow = y;
//     phases[idx].t_red = r;
// }

// void Plain_model::load()
// {
//     Preferences p;
//     String name = "plan_" + String(id);
//     p.begin(name.c_str(), false);

//     Serial.println(name);

//     String key = "START";
//     start = p.putULong(key.c_str(), 86400);
//     Serial.printf("Start: %ul\n", start);

//     // Salva o tipo de pedestre do plano atual
//     key = "WALKER_TYPE";
//     type = p.getInt(key.c_str(), S_MODE::NORMAL);

//     // Salva a quantidade de canais do plano atual
//     key = "N_PHASES";
//     n_phases = p.getInt(key.c_str(), 1);

//     for (int i = 0; i < 8; i++)
//     {
//         Phase_model phase;

//         key = "TYPE";
//         phase.type = p.getInt(key.c_str(), PHASE_TYPE::CARS);

//         key = "CH{}_GREEN";
//         key.replace("{}", String(i));
//         phase.t_green = p.getInt(key.c_str(), 1);
//         Serial.printf("Verde: %s: %d\t ", key.c_str(), p.getInt(key.c_str(), 1));

//         key = "CH{}_YELLOW";
//         key.replace("{}", String(i));
//         phase.t_yellow = p.getInt(key.c_str(), 1);
//         Serial.printf("Amarelo: %s: %d\t", key.c_str(), p.getInt(key.c_str(), 1));

//         key = "CH{}_RED";
//         key.replace("{}", String(i));
//         phase.t_red = p.getInt(key.c_str(), 1);
//         Serial.printf("Vermelho: %s: %d\n", key.c_str(), p.getInt(key.c_str(), 1));

//         phases.push_back(phase);
//     }
//     p.end();
// }

// void Plain_model::save()
// {
//     Preferences p;

//     String name = "plan_" + String(id);

//     Serial.printf("Salvando plano: %s\n", name.c_str());

//     p.begin(name.c_str(), false);

//     String key = "START";
//     p.putULong(key.c_str(), start);


//     // Salva o tipo de pedestre do plano atual
//     key = "WALKER_TYPE";
//     p.putInt(key.c_str(), type);

//     // Salva a quantidade de canais do plano atual
//     key = "N_PHASES";
//     p.putInt(key.c_str(), n_phases);

//     for (int i = 0; i < 8; i++)
//     {
//         key = "TYPE";
//         p.putInt(key.c_str(), phases[i].type);

//         key = "CH{}_GREEN";
//         key.replace("{}", String(i));
//         p.putInt(key.c_str(), phases[i].t_green);
//         Serial.printf("Verde: %s: %d\t ", key.c_str(), p.getInt(key.c_str(), 1));

//         key = "CH{}_YELLOW";
//         key.replace("{}", String(i));
//         p.putInt(key.c_str(), phases[i].t_yellow);
//         Serial.printf("Amarelo %s: %d\t", key.c_str(), p.getInt(key.c_str(), 1));

//         key = "CH{}_RED";
//         key.replace("{}", String(i));
//         p.putInt(key.c_str(), phases[i].t_red);
//         Serial.printf("Vermelho: %s: %d\n", key.c_str(), p.getInt(key.c_str(), 1));
//     }
//     p.end();
// }

// void Plain_model::startPlain()
// {
//     xTaskCreate(this->run, "CURR_PLAIN_TASK", 4048, (void *)this, 1, NULL); // 4048
// }

// void Plain_model::run(void *parameter)
// {}



