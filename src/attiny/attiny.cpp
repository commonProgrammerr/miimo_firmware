#ifdef __AVR_ATtiny85__
#include "attiny.h"

void __awake_esp__(uint8_t mode = 0U)
{
  digitalWrite(PB0, LOW);
  digitalWrite(AWAKE_ESP_PIN, HIGH);
  if (mode > 0)
  {
    digitalWrite(PB1, mode % 2);
    delay(600);
    digitalWrite(PB0, HIGH);
    delayMicroseconds(50);
    digitalWrite(PB0, LOW);
    delayMicroseconds(50);
  }
  esp_awake = true;
}

void blink(int times = 1)
{
  if (times <= 0)
    return;

  for (byte i = 0; i < times; i++)
  {
    digitalWrite(PB1, !digitalRead(PB1));
    delay(400);
    digitalWrite(PB1, !digitalRead(PB1));
    delay(400);
  }
}

bool debounce()
{
  // if (analogRead(A3) < 800)
  //   return false;
  // delay(3000);
  return analogRead(A3) >= 800;
}

void setup()
{
  setup_pins();
  Dump.begin();
}
void loop()
{
  if (esp_awake && Dump.available())
  {
    blink(Dump.read());
    delay(2000);
  }
  else if (esp_awake)
    __sleep_esp__();

  if (debounce())
    __awake_esp__(1);
}

#endif