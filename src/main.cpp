#include <Arduino.h>

#define SLEEP_STEP 500

#ifdef ESP8266
#include "stand/stand.h"

void esp_hard_sleep();

void setup()
{
  config();
  log_value("[main]: saved status = ", saved_sensor_status);
  byte status_readed = sensor();
  log_value("[main]: current status = ", status_readed);
  if (status_readed != saved_sensor_status)
  {
    update_server(status_readed);
    saved_sensor_status = status_readed;
    save_param("status", String(saved_sensor_status));
    delay(10);
  }

  esp_hard_sleep();
}

void loop()
{
}

void esp_hard_sleep()
{
  logln(F("going to sleep..."));
  writeWak(HIGH);
}
#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"
#include "pins.h"
#include "times.h"

#define AWAKE_VALUE 380

uint8_t espec_value = HIGH;
uint64_t time = millis();

#define await_esp(value) \
  time = millis();       \
  while (readWak() == !value && seconds_ms(20) >= (millis() - time))

#define __sleep_esp__() \
  digitalWrite(AWAKE_ESP_PIN, LOW)

#define __awake_esp__() \
  digitalWrite(AWAKE_ESP_PIN, HIGH)

int sensor_read()
{
  int read = sensor_value;
  delayMicroseconds(20);
  read += sensor_value;
  delayMicroseconds(20);
  read += sensor_value;

  return static_cast<int>(read / 3);
}

#define get_sensor_status() \
  sensor_read() >= AWAKE_VALUE ? HIGH : LOW

bool is_configure(byte time, int value = 820)
{
  if (sensor_read() >= value)
    for (uint8_t i = 0; i < 10; i++)
    {
      if (sensor_read() < value)
        return false;
      snore(static_cast<uint32_t>(seconds_ms(time / 10)));
    }

  return sensor_read() >= value;
}
bool debounce_value()
{
  if (sensor_read() >= 820)
    return false;

  if (get_sensor_status() == espec_value)
    snore(seconds_ms(15));
  return get_sensor_status() == espec_value;
}

void setup()
{

  setup_pins();

  __awake_esp__();
  await_esp(HIGH)
  {
    snore(200);
  }
  __sleep_esp__();
}

void loop()
{

  snore(SLEEP_STEP);

  if (is_configure(5, 820))
  {
    writeCH1(HIGH);
    __awake_esp__();
    await_esp(HIGH)
    {
      snore(1000);
    }

    writeCH1(LOW);
    __sleep_esp__();
  }

  else if (debounce_value())
  {

    espec_value = !get_sensor_status();
    __awake_esp__();
    await_esp(HIGH)
    {
      snore(1000);
    }
    __sleep_esp__();
  }
}
#endif