#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include "led_control.h"
#include "status_server.h"
#include "connection.h"

#include "user_interface.h"
#include "gpio.h"

#define BUTTON_PIN 0

byte buttonState = 0;

void get_pin_status_update_server(long time_delay = 2000)
{
  byte newButtonState = digitalRead(BUTTON_PIN);
  if (buttonState == newButtonState)
    return;
  delay(time_delay);
  if (newButtonState == digitalRead(BUTTON_PIN))
  {
    buttonState = newButtonState;
    Serial.println("contact!");
    if (WiFi.status() != WL_CONNECTED)
    {
      WiFi.forceSleepWake();
      delay(10);
    }
    if (wifi_reconnect())
    {

      Serial.println("Enviando...");
      Serial.print("Pin state: ");
      Serial.println(buttonState);
      if (update_status_on_server(buttonState, getDeviceName().substring(6)))
      {
        Serial.println("Sucesso!");
      }
      else
      {
        Serial.println("Error!");
      }
    }
    else
    {
      ESP.reset();
    }
  }
  else
  {
    Serial.print('.');
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);
  pinMode(BUTTON_PIN, INPUT);
  gpio_init();
  Serial.flush();
  Serial.print('\n');
  wifi_connection_setup();
  Serial.flush();
}

void loop()
{
  get_pin_status_update_server();
  if ((WiFi.status() == WL_CONNECTED))
  {
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    Serial.println("WiFi is down");
    delay(10);
  }
  else
  {
    Serial.print("Pin state: ");
    Serial.println(digitalRead(BUTTON_PIN));
    delay(300);
  }
}