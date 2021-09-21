
#ifndef _LED_CONTROL_H

#define _LED_CONTROL_H
#include <Arduino.h>
#include <Ticker.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 1 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

void led_tick();
void led_OFF();
void led_ON();
void led_tick_start(float seconds);
void led_tick_stop();
#endif
