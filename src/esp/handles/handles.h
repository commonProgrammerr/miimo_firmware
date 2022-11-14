#ifdef ESP8266
#ifndef __handles_h
#define __handles_h

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <dump_serial.h>

#include "logs.h"
#include "status_server.h"
#include "connection.h"
#include "SoftwareSerial.h"
#include "pins.h"
#include "times.h"

#define CLEAN_CODE 0
#define ALARM_CODE 1
#define FALSE_ALARM_CODE 3

#define sensor() digitalRead(SENSOR_PIN)

extern volatile float time_delay;
extern volatile byte saved_sensor_status;

void handle_wifi_configuration(Stream &reg = Serial);
bool update_server(byte code);
void ping_attiny(uint32 delay = 200);
void send_status();

#endif
#endif