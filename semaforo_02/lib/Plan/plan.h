#ifndef PLAIN_H
#define PLAIN_H
#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>

#define PLAIN_MODEL_NAMESPACE "PLANOS"
namespace S_MODE
{
    enum e
    {
        NORMAL,
        HIGH_DEMAND
    };
}

class Plan_model
{
public:
    int id;   // Plano1, plano2, ...
    int type; // Normal, alta demanda
    int n_phases;
    int start_hour = 0;
    int start_min = 0;
    int end_hour = 0;
    int end_min = 0;
    int start_day = 1;
    int end_day = 1;
    int delays[8][3];
    bool active = 0;
    uint32_t start_counter;
    uint32_t stop_counter;
  
    void save() {}
    void load() {}
    void startPlain() {}
    static void run(void *parameter) {}
};

#endif