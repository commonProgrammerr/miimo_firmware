#ifdef __AVR_ATtiny85__
#include "attiny.h"

void setup()
{
  setup_pins();
  Dump.begin();
}

void loop()
{
  if (IS_SETUP_MODE)
    return;
  else if (IS_CONFIGURE_MODE)
  {
    delay(100);
    int read;
    while ((!Dump.available() || read <= 0))
    {
      read = Dump.read();
      if (millis() - time >= seconds_ms(150))
        return;
    }

    timer = read / 10;
    __sleep_esp();
  }
  else if (IS_SLEEPING_MODE)
  {
    if (debounce_value(5, CONFIGURE_VALUE, HIGH))
      __awake_esp__(HIGH);
    else if (debounce_value(timer, AWAKE_VALUE))
    {
      __awake_esp__();
      espec_value = !espec_value;
    }
    snore(500);
  }
  else if (static_cast<signed long>(millis() - time) >= seconds_ms(30))
    __sleep_esp();
}

ISR(INT0_vect)
{
  if (IS_CONFIGURE_MODE)
    handle_store_bytes();
  else if (IS_AWAKED_MODE)
    __sleep_esp();
  else if (IS_SETUP_MODE)
  {
    esp_mode = configure_mode ? configure_flag : awaked_flag;
  }
}
#endif