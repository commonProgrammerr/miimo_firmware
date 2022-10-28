#ifndef __pins_h
#define __pins_h

#define SENSOR_PIN 4U
#define RESTORE_PIN 2U
#define RX_PIN 14U
#define TX_PIN 5U

#define SLEEP_PIN 5U   // SAIDA: serial to attiny
#define ADC_SIWTCH 16U // SAIDA: Comando para ler bateria
#define ADC_READ A0

#define setup_pins()                  \
  pinMode(SENSOR_PIN, INPUT_PULLUP);  \
  pinMode(RESTORE_PIN, INPUT_PULLUP); \
  pinMode(ADC_SIWTCH, OUTPUT);        \
  digitalWrite(ADC_SIWTCH, LOW);      \
  // pinMode(SLEEP_PIN, OUTPUT);         \
  // digitalWrite(SLEEP_PIN, LOW)

#endif