#ifndef __pins_h
#define __pins_h
#include <Arduino.h>
#ifdef ESP8266
#define SENSOR_PIN 13U
#define KEY_PIN 2U

#define RX_PIN 14U
#define TX_PIN 5U

#define SLEEP_PIN 5U   // SAIDA: serial to attiny
#define ADC_SIWTCH 16U // SAIDA: Comando para ler bateria
#define ADC_READ A0

#define setup_pins()              \
  pinMode(TX_PIN, OUTPUT);        \
  pinMode(ADC_SIWTCH, OUTPUT);    \
  digitalWrite(ADC_SIWTCH, LOW);  \
  pinMode(RX_PIN, INPUT_PULLUP);  \
  pinMode(KEY_PIN, INPUT_PULLUP); \
  pinMode(SENSOR_PIN, INPUT_PULLUP)

#define __interrupt__(pin, func, mode) \
  attachInterrupt(digitalPinToInterrupt(pin), func, mode)
#endif
#ifdef __AVR_ATtiny85__

#include "avr/interrupt.h"
#include "avr/io.h"

#define TX_PIN PB1
#define RX_PIN PB2
#define AWAKE_ESP_PIN PB4

#define setup_pins()              \
  pinMode(AWAKE_ESP_PIN, OUTPUT); \
  pinMode(PB0, OUTPUT);           \
  pinMode(PB1, OUTPUT);           \
  pinMode(PB2, INPUT_PULLUP);     \
  digitalWrite(PB1, 1);

#endif
#endif