#ifndef __STAND_H
#define __STAND_H
#ifdef ESP8266

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include "../logs/logs.h"
#include "status_server.h"
#include "connection.h"
// extern "C"
// {
// #include "gpio.h"
// #include "user_interface.h"
// }

#define SENSOR_PIN 4U
#define RESTORE_PIN 2U

#define SLEEP_PIN 5U   // SAIDA: serial to attiny
#define ADC_SIWTCH 16U // SAIDA: Comando para ler bateria
// #define LED_PIN 4U
#define gpioRead(pin) (digitalRead(pin))
#define gpioWrite(pin, value)                                  \
  (digitalWrite(pin, value == HIGH ? LOW : value == LOW ? HIGH \
                                                        : value))
#define setup_pins()                  \
  pinMode(SENSOR_PIN, INPUT_PULLUP);  \
  pinMode(RESTORE_PIN, INPUT_PULLUP); \
  pinMode(ADC_SIWTCH, OUTPUT);        \
  digitalWrite(ADC_SIWTCH, LOW);      \
  pinMode(SLEEP_PIN, OUTPUT);         \
  gpioWrite(SLEEP_PIN, LOW)

#define CLEAN_CODE 0
#define ALARM_CODE 1
#define FALSE_ALARM_CODE 3

// #define SAVE_IN_RTC_MEMO(status, memo) system_rtc_mem_write(memo, &status, sizeof(status));
// #define READ_OF_RTC_MEMO(status, memo) system_rtc_mem_read(memo, &status, sizeof(status));
// #define SAVE_IN_RTC(status) (SAVE_IN_RTC_MEMO(status, 64));
// #define READ_OF_RTC(status) (READ_OF_RTC_MEMO(status, 64));

// #define gpioRead(pin) (GPIO_INPUT_GET(GPIO_ID_PIN(pin)))
// #define gpioWrite(pin, value) (GPIO_OUTPUT_SET(GPIO_ID_PIN(pin), value))
#define sensor() (digitalRead(SENSOR_PIN))
#define await(t) (delay(static_cast<long>(t)))

// byte get_sensor_status(byte last_status);
byte get_sensor_status(byte last_status, long max);
byte get_debounced_status(float time);
bool update_server(byte code);
// void led_on();
// void led_off();
#endif
#endif