#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include "led_control.h"
#include "status_server.h"
#include "connection.h"

#include "user_interface.h"
#include "gpio.h"

#define BUTTON_PIN 0

byte buttonState = 0;
float time_delay = 2.0f; 
  
void get_pin_status_update_server()
{
  byte newButtonState = digitalRead(BUTTON_PIN);
  led_ON();
  if (buttonState == newButtonState)
    return;
  delay(static_cast<long>(time_delay * 1000));
  if (newButtonState == digitalRead(BUTTON_PIN))
  {
    led_tick_start(0.2);
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
  led_OFF();
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);
  pinMode(BUTTON_PIN, INPUT);
  gpio_init();
  Serial.print('\n');
  wifi_connection_setup();
  Serial.flush();
  time_delay = get_saved_param("delay").toFloat();
}

void loop()
{
  get_pin_status_update_server();
  if ((WiFi.status() == WL_CONNECTED))
  {
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    Serial.println("[main]: WiFi is down");
    delay(10);
  }
  else
  {
    Serial.print("[main]: Pin state: ");
    Serial.println(digitalRead(BUTTON_PIN));
    delay(300);
  }
}