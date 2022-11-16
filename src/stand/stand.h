#ifndef __STAND_H
#define __STAND_H
#ifdef ESP8266

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "logs.h"
#include "pins.h"
#include "status_server.h"
#include "connection.h"

#define CLEAN_CODE 0
#define ALARM_CODE 1
#define FALSE_ALARM_CODE 3

#define sensor() (!digitalRead(SENSOR_PIN))
#define await(t) (delay(static_cast<long>(t)))

extern byte saved_sensor_status;
bool update_server(byte code);
void handle_wifi_configuration();
void config();

#endif
#endif