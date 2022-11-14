#ifdef __AVR_ATtiny85__
#ifndef __handles_h
#define __handles_h
#include <Arduino.h>
#include "tinysnore.h"
#include <times.h>
#include <pins.h>
#include <dump_serial.h>

#define AWAKE_VALUE 380
#define CONFIGURE_VALUE 820

#define setup_flag 0x11     // esp acordado em setup
#define awaked_flag 0x22    // esp acordado e pos setup
#define sleeping_flag 0x33  // enquanto o esp dorme
#define configure_flag 0x44 // esp acordado, pos setup, em modo de configuração

extern volatile byte esp_mode;
extern volatile bool configure_mode;

#define SETUP_MODE esp_mode = 0x11
#define AWAKED_MODE esp_mode = 0x22
#define SLEEPING_MODE esp_mode = 0x33
#define CONFIGURE_MODE esp_mode = 0x44

#define IS_SETUP_MODE esp_mode == 0x11
#define IS_AWAKED_MODE esp_mode == 0x22
#define IS_SLEEPING_MODE esp_mode == 0x33
#define IS_CONFIGURE_MODE esp_mode == 0x44

#define get_sensor_status(value) \
  sensor_read() >= value ? HIGH : LOW

extern float timer;
extern uint64_t time;
extern byte espec_value;

void ping_esp();
int sensor_read();

void __sleep_esp();
void __awake_esp__(byte configure = LOW);

bool debounce_value(float time, int value, byte espec = espec_value);

#endif
#endif