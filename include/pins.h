#ifndef __pins_h
#define __pins_h
#include <Arduino.h>
#ifdef ESP8266

#define WAK_PIN 5U
#define CH_1_PIN 14U
#define CH_2_PIN 2U
#define SENSOR_PIN 13U

#define ADC_SIWTCH 16U // SAIDA: Comando para ler bateria
#define ADC_READ A0

#define setup_pins()               \
  pinMode(WAK_PIN, OUTPUT);        \
  pinMode(ADC_SIWTCH, OUTPUT);     \
  digitalWrite(ADC_SIWTCH, LOW);   \
  pinMode(CH_1_PIN, INPUT_PULLUP); \
  pinMode(CH_2_PIN, INPUT_PULLUP); \
  pinMode(SENSOR_PIN, INPUT_PULLUP)

#define sensor_value analogRead(SENSOR_PIN)
#define writeWak(value) digitalWrite(WAK_PIN, value)
#define readCH1() digitalRead(CH_1_PIN) ? LOW : HIGH
#define readCH2() digitalRead(CH_2_PIN) ? LOW : HIGH
#define CH1 digitalRead(CH_1_PIN)
#define CH2 digitalRead(CH_2_PIN)

#define __interrupt__(pin, func, mode) \
  attachInterrupt(digitalPinToInterrupt(pin), func, mode)
#endif
#ifdef __AVR_ATtiny85__

#include "avr/interrupt.h"
#include "avr/io.h"

#define SENSOR_PIN A3
#define CH_1_PIN PB1
#define CH_2_PIN PB0
#define WAK_PIN PB2
#define AWAKE_ESP_PIN PB4

#define setup_pins()              \
  pinMode(AWAKE_ESP_PIN, OUTPUT); \
  pinMode(CH_1_PIN, OUTPUT);      \
  pinMode(CH_2_PIN, OUTPUT);      \
  pinMode(WAK_PIN, INPUT_PULLUP)

#define sensor_value analogRead(SENSOR_PIN)
#define readWak() !digitalRead(WAK_PIN)
#define writeCH1(value) digitalWrite(CH_1_PIN, value)
#define writeCH2(value) digitalWrite(CH_2_PIN, value)

#endif
#endif