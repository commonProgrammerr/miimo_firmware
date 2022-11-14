#ifndef __esp_h__
#define __esp_h__

#ifdef ESP8266
#include <Arduino.h>
#include "handles/handles.h"

void IRAM_ATTR handle_configure();
void IRAM_ATTR handle_status();

void setup();
void loop();

#define DEFAULT_MODE 0x11
#define CONFIG_MODE 0x22

#endif

#endif