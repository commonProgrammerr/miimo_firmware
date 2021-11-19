#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include "status_server.h"
#include "connection.h"

#include "user_interface.h"
#include "gpio.h"

#define SENSOR_PIN 0
#define RESTORE_PIN 2
#define RECURRENCE_CODE 3

#define Log(message) (Serial ? Serial.print(message) : 0)
#define Logln(message) (Serial ? Serial.println(message) : 0)

byte sensor_status = 0;
bool was_alarm = false;
int counter = 0;
float time_delay = 2.0f;

bool IS_A_CRITICAL_RECURRENCE() __attribute__((always_inline));

bool inline IS_A_CRITICAL_RECURRENCE(long t)
{
  int counter_limiter = static_cast<int>(6 + (time_delay / 3));
  long min_valid_time = static_cast<long>(time_delay * 920);
  return (counter >= counter_limiter && t < min_valid_time);
}

byte get_sensor_status()
{
  byte current_sensor_status = digitalRead(SENSOR_PIN);
  if (sensor_status == current_sensor_status)
    return sensor_status;
  else
  {
    for (long t = 0; t < time_delay * 1000; t += 50)
    {
      delay(50);
      if (IS_A_CRITICAL_RECURRENCE(t))
      {
        counter = 0;
        return RECURRENCE_CODE;
      }
      else if (digitalRead(SENSOR_PIN) != current_sensor_status)
      {
        counter++;
        current_sensor_status = digitalRead(SENSOR_PIN);
      }
    }
    counter = 0;
    return current_sensor_status;
  }
}

void update_server(byte code)
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.forceSleepWake();
    delay(10);
    Serial.println("[main]: Ligando wifi...");
  }
  if (wifi_reconnect())
  {
    Serial.println("[main]: Conectado!");
    Serial.println("[main]: Enviando...");
    Serial.print("[main]: Sensor status: ");
    Serial.println(code);
    if (update_status_on_server(code, getDeviceName().substring(6)))
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

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(RESTORE_PIN, INPUT);
  // gpio_init();
  params_start_FS();
  Serial.print('\n');
  wifi_connection_setup();
  Serial.flush();
  time_delay = get_saved_param("delay").toFloat();
}

void loop()
{
  byte status_readed = get_sensor_status();
  if (status_readed == RECURRENCE_CODE)
  {
    update_server(RECURRENCE_CODE);
    sensor_status = digitalRead(SENSOR_PIN);
    was_alarm = true;
  }
  else if (status_readed != sensor_status)
  {
    sensor_status = status_readed;
    was_alarm = false;
    update_server(status_readed);
  }
  if ((WiFi.status() == WL_CONNECTED))
  {
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    Serial.println("[main]: WiFi is down");
    delay(10);
  }
  else if(digitalRead(RESTORE_PIN) == 0)
  {
    delay(15000);
    Serial.print("[main]: Reset_pin -> ");
    bool should_reset = digitalRead(RESTORE_PIN) == 0;
    Serial.println(should_reset ? "True" : "False");
    if(should_reset) {
      Serial.println("[main]: Reseting configurations...");
      WiFiManager wm;
      wm.resetSettings();
      ESP.reset();
    }
  }
  else
  {
    Serial.print("[main]: Sensor status: ");
    Serial.println(digitalRead(SENSOR_PIN));
    delay(300);
  }
}