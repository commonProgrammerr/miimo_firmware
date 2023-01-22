#include <Arduino.h>

#define SLEEP_STEP 500
// float toFloat(uint8_t x);
// uint8_t fromFloat(float x);

#ifdef ESP8266
#include "stand/stand.h"

void esp_hard_sleep();

void setup()
{
  config();
  byte status_readed = sensor();
  log_value("[main]: saved status = ", saved_sensor_status);
  log_value("[main]: current status = ", status_readed);
  update_server(status_readed);

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
#define CONFIG_VALUE 920

uint8_t espec_value = HIGH;
float debounce_timer = 15.0f;
uint64_t time = millis();
uint64_t count = 0UL;

#define await_esp(value) \
  time = millis();       \
  while (readWak() == !value && seconds_ms(20) >= (millis() - time))

#define __iftimeout__(start, timeout) if (millis() - start >= timeout)

#define __sleep_esp__()             \
  digitalWrite(AWAKE_ESP_PIN, LOW); \
  resetCH();                        \
  count = 0Ul

#define __awake_esp__() \
  digitalWrite(AWAKE_ESP_PIN, HIGH)

#define get_sensor_status() \
  AWAKE_VALUE <= sensor_read() ? HIGH : LOW

// bool read_byte(byte *data)
// {
//   // uint32_t time = millis();

//   while (wak_value == HIGH)
//     delayMicroseconds(5);

//   while (wak_value == LOW)
//     delayMicroseconds(5);

//   delayMicroseconds(20);

//   for (byte i = 0; i < 8; i++)
//   {
//     delayMicroseconds(100);
//     (*data) |= digitalRead(WAK_PIN) << i;
//     delayMicroseconds(100);
//   }

//   return true;
// }

int sensor_read();
void send_signal();
bool validate_value();
bool validate_config();
bool debounce(bool (*validator)(void), float time = 0.0);

void setup()
{
  setup_pins();
  __awake_esp__();
  await_esp(HIGH) snore(200);
  __sleep_esp__();
}

void loop()
{

  snore(SLEEP_STEP);

  if (count >= static_cast<uint64_t>(hours_ms(4) / SLEEP_STEP))
    send_signal();

  else if (debounce(validate_config, 5.0f))
  {
    writeCH1(HIGH);
    send_signal();
  }
  else if (debounce(validate_value, debounce_timer))
  {
    espec_value = !espec_value;
    send_signal();
  }
}

int sensor_read()
{
  int read = sensor_value;
  delayMicroseconds(20);
  read += sensor_value;
  delayMicroseconds(20);
  read += sensor_value;

  return static_cast<int>(read / 3);
}

void send_signal()
{
  __awake_esp__();
  await_esp(HIGH)
      snore(SLEEP_STEP);
  __sleep_esp__();
}

bool validate_config() { return sensor_read() >= CONFIG_VALUE; }

bool validate_value() { return validate_config() ? false : get_sensor_status() == espec_value; }

bool debounce(bool (*validator)(void), float time)
{
  if (validator())
    for (uint8_t i = 0; i < 10; i++)
    {
      if (validator() == false)
        return false;
      snore(static_cast<uint32_t>(seconds_ms(time / 10)));
    }

  return validator();
}

#endif

// float toFloat(uint8_t x)
// {
//   return x / 255e1;
// }

// uint8_t fromFloat(float x)
// {
//   if (x < 0)
//     return 0;
//   if (x > 10)
//     return 255;
//   return 255e1 * x; // this truncates; add 0.5 to round instead
// }