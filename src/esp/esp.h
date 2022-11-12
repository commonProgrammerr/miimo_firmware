#ifndef __esp_h__
#define __esp_h__

#include <Arduino.h>

#include <times.h>
#include <pins.h>
#include <dump_serial.h>

#ifdef ESP8266
#include "stand.h"

void IRAM_ATTR handle_configure();

void setup();
void loop();

#define SETUP_MODE ((digitalRead(14) == LOW))
#define RESET_MODE ((digitalRead(14) == HIGH))
#define SETUP 0x14
#define SLEEP 0x20

#endif

#endif