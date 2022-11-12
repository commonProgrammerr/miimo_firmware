#ifndef attiny_h
#define attiny_h
#ifdef __AVR_ATtiny85__
#include <Arduino.h>
#include <times.h>
#include <pins.h>
#include <dump_serial.h>
#include "tinysnore.h"

static bool esp_awake = false;

#define __sleep_esp__()             \
  digitalWrite(AWAKE_ESP_PIN, LOW); \
  esp_awake = false;

void __awake_esp__(uint8_t mode = 0U);

void blink(byte times = 1);

bool debounce();

void setup();

void loop();

#endif
#endif