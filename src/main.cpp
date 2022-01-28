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

#define SENSOR_PIN D5
#define RESTORE_PIN LED_BUILTIN
#define CLEAN_CODE 0
#define ALARM_CODE 1
#define FALSE_ALARM_CODE 3
#define DEBUG_LOG
byte saved_sensor_status = 10;
// #define SET_SENSOR_STATUS (status) sensor_status = status; system_rtc_mem_write(64, &sensor_status, 4);
// #define UPDATE_SENSOR_STATUS (status) system_rtc_mem_read(64, &sensor_status, 4);

#ifdef DEBUG_LOG
#define log(msg) (Serial.println(msg))
#define log_value(msg, data) ((Serial.print((msg))), (log((data))), (void)0)
#else
#define log_value(msg, data) (void)0
#define log(msg) (void)0
#endif

#define gpioRead(pin) (GPIO_INPUT_GET(GPIO_ID_PIN(pin)))
// #define gpioRead(pin) (digitalRead(pin))
#define sensor() (gpioRead(SENSOR_PIN))
#define await(t) (delay(static_cast<long>(t)))

bool was_false_alarm = false;
float time_delay = 2.0f;

byte get_sensor_status()
{

  byte read[] = {10, 10, 10};
  read[0] = sensor();
  log_value("[status]: sensor_read[0] = ", read[0]);

  if (read[0] == saved_sensor_status)
    return read[0];

  await(time_delay * (160 + ((read[0] == LOW) ? 200 : 700)));

  read[1] = sensor();
  log_value("[status]: sensor_read[1] = ", read[1]);

  if (read[0] != LOW)
  {
    await(time_delay * 140);
    read[2] = sensor();
    log_value("[status]: sensor_read[2] = ", read[2]);

    return (read[1] == read[0] && read[2] != read[0])
               ? FALSE_ALARM_CODE
               : read[2];
  }

  return read[1];
}

bool update_server(byte code)
{
  WiFi.mode(WIFI_STA);
  if (wifi_reconnect())
  {
    log("[main]: Conectado!");
    log("[main]: Enviando...");
    log_value("[main]: Sensor status: ", code);
    if (update_status_on_server(code, getDeviceName().substring(6)))
    {
      log("Sucesso!");
      return true;
    }
    else
    {
      log("Error!");
      return false;
    }
  }
  else
  {
    log("[WiFi]: Wifi indisponivel!");
    return false;
  }
}

void wakeup_callback()
{
  log("[wakeup]: Wake!");
  log_value("[wakeup]: delay time = ", time_delay);
  log_value("[wakeup]: last status = ", saved_sensor_status);
  GPIO_OUTPUT_SET(GPIO_ID_PIN(RESTORE_PIN), LOW);
}

void sleep()
{
  WiFi.mode(WIFI_OFF);

  log("[sleep]: Going to sleep now...");
  delay(3);

  wifi_station_disconnect();
  wifi_set_opmode_current(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_set_wakeup_cb(wakeup_callback);
  if (sensor() == 0)
  {
    log("[sleep]: Set wakeup mode to HILEVEL");
    delay(3);
    gpio_pin_wakeup_enable(GPIO_ID_PIN(SENSOR_PIN), GPIO_PIN_INTR_HILEVEL);
  }
  else
  {
    log("[sleep]: Set wakeup mode to LOLEVEL");
    delay(3);
    gpio_pin_wakeup_enable(GPIO_ID_PIN(SENSOR_PIN), GPIO_PIN_INTR_LOLEVEL);
  }
  wifi_fpm_open();
  wifi_fpm_do_sleep(0xFFFFFFF);
  delay(100);
}

void setup()
{
  Serial.begin(115200);
  gpio_init(); // Initilise GPIO pins
  delay(1000);
  params_start_FS();
#ifdef ENV_NODEMCU
  Serial.println
  ('teste');
  delay(100);
#endif
  wifi_connection_setup();
  Serial.flush();
  time_delay = get_saved_param("delay").toFloat();
}

void loop()
{
  byte status_readed = get_sensor_status();
  log_value("[main]: final status = ", status_readed);

  if (status_readed == FALSE_ALARM_CODE)
  {
    if (update_server(status_readed))
      saved_sensor_status = sensor();
  }
  else if (status_readed != saved_sensor_status)
  {
    if (update_server(status_readed))
      saved_sensor_status = status_readed;
  }

  sleep();
}