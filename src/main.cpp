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
#define CLEAN_CODE 0
#define ALARM_CODE 3
#define FALSE_ALARM_CODE 3
#define DEBUG_LOG
byte saved_sensor_status = 0;
// #define SET_SENSOR_STATUS (status) sensor_status = status; system_rtc_mem_write(64, &sensor_status, 4);
// #define UPDATE_SENSOR_STATUS (status) system_rtc_mem_read(64, &sensor_status, 4);

#ifdef DEBUG_LOG
#define debug_log(msg, data) ((Serial.print((msg))), (Serial.println((data))), (void)0)
#else
#define debug_log(msg, data) (void)0
#endif

bool was_false_alarm = false;
float time_delay = 2.0f;

byte get_sensor_status()
{
  delay(200);
  byte first_read = GPIO_INPUT_GET(SENSOR_PIN);
  debug_log("[debug]: sensor pin first read = ", first_read);

  if (first_read == saved_sensor_status)
    return first_read;

  delay(static_cast<long>(time_delay * 860));

  byte current_sensor_status = GPIO_INPUT_GET(SENSOR_PIN);
  debug_log("[debug]: after first read, sensor pin read = ", current_sensor_status);

  bool was_pass_limit = first_read == current_sensor_status;

  delay(static_cast<long>(time_delay * 140));
  current_sensor_status = GPIO_INPUT_GET(SENSOR_PIN);
  debug_log("[debug]: after second read, sensor pin read = ", current_sensor_status);

  was_false_alarm = was_pass_limit && current_sensor_status != first_read;

  if (was_false_alarm)
    return FALSE_ALARM_CODE;

  return current_sensor_status;
}

void update_server(byte code)
{
  WiFi.mode(WIFI_STA);
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
    Serial.println("[WiFi]: Wifi indisponivel!");
  }
}

// void initWifi()
// {
//   WiFi.mode(WIFI_STA);
//   WiFi.begin("GVT-8D59", "arer3366547");
//   while ((WiFi.status() != WL_CONNECTED))
//   {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("WiFi connected, IP address: ");
//   Serial.println(WiFi.localIP());
// }

void sleep(byte wakeup_mode)
{
  byte real_wakeup_mode;
  Serial.println("[main]: Going to sleep now...");

  if (wakeup_mode == GPIO_INPUT_GET(SENSOR_PIN))
  {
    real_wakeup_mode = wakeup_mode;
  }
  else
  {
    real_wakeup_mode = GPIO_INPUT_GET(SENSOR_PIN);
  }
  delay(200);
  wifi_station_disconnect();
  wifi_set_opmode_current(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  if (real_wakeup_mode == CLEAN_CODE)
  {
    Serial.println("[main]: Set wakeup mode to HILEVEL");
    delay(200);
    gpio_pin_wakeup_enable(GPIO_ID_PIN(SENSOR_PIN), GPIO_PIN_INTR_HILEVEL);
  }
  else
  {
    Serial.println("[main]: Set wakeup mode to LOLEVEL");
    delay(200);
    gpio_pin_wakeup_enable(GPIO_ID_PIN(SENSOR_PIN), GPIO_PIN_INTR_LOLEVEL);
  }
  wifi_fpm_do_sleep(0xFFFFFFF); // Sleep for longest possible time
}

void setup()
{
  Serial.begin(115200);
  gpio_init(); // Initilise GPIO pins
  delay(100);
  params_start_FS();
  Serial.print('\n');
  wifi_connection_setup();
  Serial.flush();
  time_delay = get_saved_param("delay").toFloat();
  // saved_sensor_status = GPIO_INPUT_GET(SENSOR_PIN);
}

void loop()
{
  Serial.println("[main]: Wake!");

  debug_log("[debug]: delay time = ", time_delay);
  debug_log("[debug]: last status = ", saved_sensor_status);
  byte status_readed = get_sensor_status();
  debug_log("[debug]: final status = ", status_readed);
  if (status_readed == FALSE_ALARM_CODE)
  {
    update_server(FALSE_ALARM_CODE);
    delay(200);
    sleep(GPIO_INPUT_GET(SENSOR_PIN));
    delay(200);
  }
  else if (status_readed != saved_sensor_status)
  {
    saved_sensor_status = status_readed;
    update_server(status_readed);
    delay(200);
    sleep(saved_sensor_status);
    delay(200);
  }
  else
  {
    delay(200);
    sleep(GPIO_INPUT_GET(SENSOR_PIN));
    delay(200);
  }
}