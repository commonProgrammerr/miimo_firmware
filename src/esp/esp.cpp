
#ifdef ESP8266
#include "esp.h"
volatile bool interrupt = false;
void setup()
{
  setup_pins();
  __interrupt__(KEY_PIN, handle_configure, FALLING);

  Serial.begin(115200);
  Serial.println();
  yield();
}
void loop()
{
}

void IRAM_ATTR handle_configure()
{
  if (interrupt)
    return;

  interrupt = true;
  if (SETUP_MODE)
  {
    Serial.println("\nSetup mode active:");
    Serial.print("  - send timer debounce: ");
    Dump.write(0b00001001);
    Dump.write(0b00000010);
    // Dump.write(0b00000101);
    Dump.write(0b00000011);
    // Dump.write(0b00001110);
  }
  else if (RESET_MODE)
  {
    Serial.println("\nReset mode active:");
    Serial.print("  - send timer debounce: ");
  }
  interrupt = false;
}

#endif