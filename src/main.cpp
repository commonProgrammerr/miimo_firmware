#include <Arduino.h>
#include <stand.h>

#define SLEEP_TIME_SECONDS 120
#define SLEEP_TIME_MICROSECONDS SLEEP_TIME_SECONDS * 1e6
#define SLEEP_TIME_GOAL ((60 * 60 * 2) / SLEEP_TIME_SECONDS) // seg * min * horas / seg
#define RTCMEMORYSTART 65
ADC_MODE(ADC_VCC);

float time_delay = 2.0f;
byte saved_sensor_status = 10;
int count = 0;
void handle_wifi_configuration()
{

  int should_config = get_saved_param("config").toInt();
  if (should_config && wifi_connection_setup())
  {
    save_param("config", String(0));
  }
  else
  {
    for (byte i = 0; gpioRead(RESTORE_PIN) && i <= 20; i++)
    {
      if (i == 20)
        wifi_reset_config();

      delay(65);
    }
  }
}

void config()
{
  Serial.begin(115200);
  gpio_init();
  WiFi.forceSleepBegin();
  delay(1);
  WiFi.forceSleepWake();
  delay(1);
  params_start_FS();
  delay(10);
  Serial.flush();
  handle_wifi_configuration();
  saved_sensor_status = static_cast<byte>(get_saved_param("status").toInt());
  time_delay = get_saved_param("delay").toFloat();
  count = get_saved_param("count").toInt();
}

void setup()
{
  config();
  log_value("[main]: delay time = ", time_delay);
  log_value("[main]: saved status = ", saved_sensor_status);
  byte status_readed = get_sensor_status(time_delay, saved_sensor_status);
  log_value("[main]: final status = ", status_readed);

  if (status_readed == FALSE_ALARM_CODE && count >= SLEEP_TIME_GOAL)
  {
    if (update_server(status_readed))
    {
      saved_sensor_status = sensor();
      count = 0;
    }
  }
  else if (status_readed != saved_sensor_status)
  {
    update_server(status_readed);
    saved_sensor_status = status_readed;
    save_param("status", String(saved_sensor_status));
    delay(5);
  }
  save_param("count", String(count + 1));
  log("Going to sleep...");
  delay(3);
  Serial.flush();
  ESP.deepSleep(SLEEP_TIME_MICROSECONDS);
}

void loop()
{
}