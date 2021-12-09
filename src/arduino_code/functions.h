#pragma once

#include <WiFiNINA.h>
#include <RTCZero.h>
#include "arduino_secrets.h"

void connect_to_wifi();
void reconnect_wifi();
void printWifiStatus();
void rpi_send();
void discord_send(String);

extern RTCZero rtc1;
extern float h, t, p;
extern int outage, sd_problem;
