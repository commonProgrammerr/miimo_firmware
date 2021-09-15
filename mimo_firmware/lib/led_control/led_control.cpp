#include "led_control.h"

void tick()
{
  //toggle state
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // set pin to the opposite state
}

void led_tick_start(float seconds)
{
  ticker.attach(seconds, tick);
}

void led_tick_stop() {
  ticker.detach();
}

void led_OFF()
{
  digitalWrite(LED_BUILTIN, HIGH);
}
void led_ON()
{
  digitalWrite(LED_BUILTIN, LOW);
}