#include <Arduino.h>

#include <ESP8266HTTPClient.h>
#include "status_server.h"
#include "connection.h"

extern "C"
{
#include "gpio.h"
}

extern "C"
{
#include "user_interface.h"
}

#define SENSOR_PIN 0
#define RESTORE_PIN 2
#define RECURRENCE_CODE 3

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
  WiFi.mode(WIFI_STA);
  if (WiFi.status() != WL_CONNECTED)
  {
    // WiFiManager wm;
    // wm.getWiFiSSID();
    // wm.getWiFiPass();
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

void initWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin("GVT-8D59", "arer3366547");
  while ((WiFi.status() != WL_CONNECTED))
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void wakeup_callback()
{
  delay(200);
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
  // if ((WiFi.status() == WL_CONNECTED))
  // {
  //   WiFi.mode(WIFI_OFF);
  //   WiFi.forceSleepBegin();
  //   Serial.println("[main]: WiFi is down");
  //   delay(10);
  // }
  // else
  // {
  //   Serial.print("[main]: Sensor status: ");
  //   Serial.println(digitalRead(SENSOR_PIN));
  //   delay(300);
  // }
}

void sleep()
{
  byte sensor_status = digitalRead(SENSOR_PIN);
  wifi_station_disconnect();
  wifi_set_opmode_current(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();                                                                                                     // Enables force sleep
  gpio_pin_wakeup_enable(GPIO_ID_PIN(SENSOR_PIN), sensor_status == 0 ? GPIO_PIN_INTR_HILEVEL : GPIO_PIN_INTR_LOLEVEL); // GPIO_ID_PIN(2) corresponds to GPIO2 on ESP8266-01 , GPIO_PIN_INTR_LOLEVEL for a logic low, can also do other interrupts, see gpio.h above
  // gpio_pin_wakeup_enable(GPIO_ID_PIN(RESTORE_PIN), GPIO_PIN_INTR_LOLEVEL);                                             // GPIO_ID_PIN(2) corresponds to GPIO2 on ESP8266-01 , GPIO_PIN_INTR_LOLEVEL for a logic low, can also do other interrupts, see gpio.h above
  // wifi_fpm_set_wakeup_cb(wakeup_callback);
  wifi_fpm_do_sleep(0xFFFFFFF); // Sleep for longest possible time
}

void setup()
{
  Serial.begin(115200);
  gpio_init(); // Initilise GPIO pins

  params_start_FS();
  Serial.print('\n');
  wifi_connection_setup();
  Serial.flush();
  time_delay = get_saved_param("delay").toFloat();
}

void loop()
{
  if ((WiFi.status() != WL_CONNECTED))
  {
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    Serial.println("[main]: WiFi is down");
    delay(10);
  }
  delay(200);
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
  delay(200);
  Serial.println("Going to sleep now");
  sleep();
  delay(200);
  Serial.println("Wake up");
}