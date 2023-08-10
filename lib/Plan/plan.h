#ifndef PLAIN_H
#define PLAIN_H
#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
#include "SPIFFS.h"
#include "FS.h"

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
    String readFile(String path);
    bool writeFile(String path, String message);
public:
    int id;   // Plano1, plano2, ...
    int type = 0; // Normal, alta demanda
    int n_phases = 1;
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
  
    bool init(int _id);
    bool listDir();
    void save();
    bool load();
    
};

#endif