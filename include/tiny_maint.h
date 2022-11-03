#ifndef __tiny_maint_h
#define __tiny_maint_h

#include <Arduino.h>
#include "SoftwareSerial.h"
#include "pins.h"
#include "times.h"
#include "sig.h"

extern uint64_t time;
extern SoftwareSerial esp_serial;
bool was_timeout;

#ifndef MAX_AWAIT_TIME
#define MAX_AWAIT_TIME 20U // tempo maximo acordado em minutos
#endif

#ifndef AWAKE_VALUE
#define AWAKE_VALUE 380 // valor de leitura para acordar esp
#endif

#ifndef CONFIG_VALUE
#define CONFIG_VALUE 1000 // valor de leitura para acordar esp
#endif

#define check_timeout(timeout) seconds_ms(timeout) > (millis() - time)
#define check_signal(sig) esp_serial.available() && esp_serial.read() == sig

#define __await_timeout__(awaiting_for, timeout) \
  for (time = millis(), was_timeout = false; !(awaiting_for) && !was_timeout; was_timeout = check_timeout(timeout))

#define __await__(awaiting_for) __await_timeout__(awaiting_for, MAX_AWAIT_TIME)

#define __await_signal__(sig) __await__(check_signal(sig)) \
    delay(100);

#define __await_wake__() __await_signal__(AWAKED_SIG)

#define __sleep_esp__() \
  digitalWrite(ESP_SIWTCH_PIN, LOW)

#define __awake_esp__() \
  digitalWrite(ESP_SIWTCH_PIN, HIGH);

#define __reset_mode__() \
  C1(LOW);               \
  C2(HIGH);

#define __configure_mode__() \
  C1(LOW);                   \
  C2(LOW);

#define __awake_sensor_mode__() \
  C1(HIGH);                     \
  C2(LOW);

#define __awake_timeout_mode__() \
  C1(HIGH);                      \
  C2(HIGH);

#define sensor_value() analogRead(SENSOR_PIN)

#define get_status(read) \
  read >= AWAKE_VALUE ? HIGH : LOW

#define get_sensor_status() get_status(sensor_read())

int sensor_read()
{
  int read = sensor_value();
  delayMicroseconds(50);
  read += sensor_value();
  delayMicroseconds(50);
  read += sensor_value();

  return static_cast<int>(read / 3);
}

#endif