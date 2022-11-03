#ifndef __pins_h
#define __pins_h

#if defined(ESP8266)

#define C1_PIN 14U
#define C2_PIN 2U

#define SENSOR_PIN 13U
#define ADC_SIWTCH_PIN 16U // SAIDA: Comando para ler bateria
#define ADC_READ_PIN A0

#define TX_PIN 5U
#define RX_PIN -1

#define setup_pins()               \
  pinMode(C1_PIN, INPUT);          \
  pinMode(C2_PIN, INPUT);          \
  pinMode(ADC_SIWTCH_PIN, OUTPUT); \
  digitalWrite(ADC_SIWTCH_PIN, LOW)

#define C1() digitalRead(C1_PIN)
#define C2() digitalRead(C2_PIN)

#elif defined(__AVR_ATtiny85__)

#define C1_PIN PB1
#define C2_PIN PB0

#define SENSOR_PIN A3
#define ESP_SIWTCH_PIN PB4

#define TX_PIN -1
#define RX_PIN PB2

#define setup_pins()                 \
  pinMode(C1_PIN, OUTPUT);           \
  pinMode(C2_PIN, OUTPUT);           \
  pinMode(RX_PIN, INPUT);            \
  pinMode(ESP_SIWTCH_PIN, OUTPUT);   \
  digitalWrite(ESP_SIWTCH_PIN, LOW); \
  C1(LOW);                           \
  C2(LOW)

#define C1(val) digitalWrite(C1_PIN, !val)
#define C2(val) digitalWrite(C2_PIN, !val)
#else
#endif
#endif