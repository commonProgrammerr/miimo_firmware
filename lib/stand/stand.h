#ifndef __STAND_H
#define __STAND_H
#ifdef ESP8266

#include <Arduino.h>
// #include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include "../logs/logs.h"
#include "status_server.h"
#include "connection.h"
#include "SoftwareSerial.h"
#include "../../include/pins.h"
#include "../../include/times.h"

#define CLEAN_CODE 0
#define ALARM_CODE 1
#define FALSE_ALARM_CODE 3

#define sensor() (!digitalRead(SENSOR_PIN))
#define await(t) (delay(static_cast<long>(t)))

// byte get_sensor_status(byte last_status);
byte get_sensor_status(byte last_status, long max);
byte get_debounced_status(float time);
bool update_server(byte code);

// extern WiFiUDP ntpUDP;
// extern NTPClient timeClient;

extern float time_delay;
extern byte saved_sensor_status;
extern uint64_t saved_time;

extern SoftwareSerial attiny_serial;

#endif
#endif