
#ifdef __AVR_ATtiny85__
#include "handles.h"
byte espec_value = HIGH;
float timer = 15.0;
uint64_t time = millis();

volatile byte esp_mode = sleeping_flag;
volatile bool configure_mode = true;

void __sleep_esp()
{
  digitalWrite(AWAKE_ESP_PIN, LOW);
  digitalWrite(PB1, LOW);
  esp_mode = sleeping_flag;
  configure_mode = false;
}

void __awake_esp__(byte configure)
{
  esp_mode = setup_flag;
  digitalWrite(PB0, LOW);
  digitalWrite(PB1, configure);
  digitalWrite(AWAKE_ESP_PIN, HIGH);
  time = millis();
  configure_mode = !(!configure);
}

void ping_esp()
{
  digitalWrite(PB0, HIGH);
  delayMicroseconds(200);
  digitalWrite(PB0, LOW);
  delayMicroseconds(200);
}

int sensor_read()
{
  int read = analogRead(A3);
  delayMicroseconds(20);
  read += analogRead(A3);
  delayMicroseconds(20);
  read += analogRead(A3);

  return static_cast<int>(read / 3);
}

bool debounce_value(float time, int value, byte espec = espec_value)
{
  if (get_sensor_status(value) == espec)
    for (uint8_t i = 0; i > 10; i++)
    {
      if (get_sensor_status(value) != espec)
        return false;
      snore(seconds_ms(time / 10));
    }

  return get_sensor_status(value) == espec;
}

#endif