#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include "led_control.h"
#include "status_server.h"
#include "connection.h"

extern "C"
{
#include "user_interface.h"
#include "gpio.h"
}

#define BUTTON_PIN D1 // PINO GPIO0

void get_pin_status_update_server(long time_delay = 4000)
{
  byte buttonState = digitalRead(BUTTON_PIN);
  delay(time_delay);
  if (buttonState == digitalRead(BUTTON_PIN))
  {
    Serial.println("Enviando...");
    Serial.print("Pin state: ");
    Serial.println(buttonState);
    if (update_status_on_server(buttonState, getDeviceName().substring(5)))
    {
      Serial.println("Sucesso!");
    }
    else
    {
      Serial.println("Error!");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);
  pinMode(BUTTON_PIN, INPUT);
  // gpio_init();
  while (!Serial)
    ;
  Serial.flush();
  Serial.print('\n');
  wifi_connection_setup();
  get_pin_status_update_server();
  Serial.println("Indo dormir!");
  Serial.flush();
  ESP.deepSleep(1e7);
  Serial.flush();
}

void loop()
{
}