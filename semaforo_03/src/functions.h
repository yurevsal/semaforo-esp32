#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "defines.h"

void initOutputs();

void initDisplay();
void initWiFiManager();
void mqttSetup();
void process();
void reconnect(void *p);

void mainScreen(void *p);
// void menuPlans(void *p);
// void menuPed(void *p);
// void menuConfigPhases(void *p);
// void menuBlink_y(void *p);
// void menuRTC(void *p);
// void menuNTP(void *p);
// void menuConfigWiFi(void *p);

// void startTime(void *p);
// void endTime(void *p);

// void menuGetStartHour(void*p);
// void menuGetStartMin(void*p);
// void menuGetEndHour(void*p);
// void menuGetEndMin(void*p);

#endif