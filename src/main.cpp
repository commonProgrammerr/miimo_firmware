#include <Arduino.h>

#define SLEEP_STEP 500
#define sw_serial_speed 1

#include <times.h>
#include <pins.h>
#include <dump_serial.h>

#ifdef ESP8266
#include "stand.h"

void IRAM_ATTR handle_configure();

void setup()
{
  setup_pins();
  __interrupt__(KEY_PIN, handle_configure, RISING);

  Serial.begin(115200);
  Serial.println();
  yield();
  digitalWrite(TX_PIN, HIGH);
}
void loop()
{
}

#define SETUP_MODE ((digitalRead(14) == LOW))
#define RESET_MODE ((digitalRead(14) == HIGH))
#define SETUP 0x14
#define SLEEP 0x20
void IRAM_ATTR handle_configure()
{
  if (SETUP_MODE)
  {
    Serial.println("\nSetup mode active:");
    Serial.print("  - send timer debounce: ");
    Dump.write(14);
    delay(100);
    Dump.write(50);
  }
  else if (RESET_MODE)
  {
    Serial.println("\nReset mode active:");
    Serial.print("  - send timer debounce: ");
  }
}

#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"

#define __sleep_esp__() \
  digitalWrite(AWAKE_ESP_PIN, LOW)

void __awake_esp__(uint8_t mode = 0U)
{
  digitalWrite(PB0, LOW);
  digitalWrite(AWAKE_ESP_PIN, HIGH);
  delay(120);
  if (mode > 0)
  {
    digitalWrite(PB1, mode % 2);
    digitalWrite(PB0, HIGH);
    delayMicroseconds(15);
    digitalWrite(PB0, LOW);
  }
  delay(300);
}

void blink(byte times = 1)
{
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
  if (analogRead(A3) < 800)
    return false;
  delay(3000);
  return analogRead(A3) >= 800;
}

void setup()
{
  setup_pins();
  Dump.begin();
}

void loop()
{
  if (debounce())
  {

    __awake_esp__(1);
    if (size)
    {
      int blinks = Dump.read();
      while (blinks <= 0)
        blinks = Dump.read();

      if (blinks > 0)
      {
        delay(2000);
        blink(blinks);
        __sleep_esp__();
      }
    }
  }
}

#endif