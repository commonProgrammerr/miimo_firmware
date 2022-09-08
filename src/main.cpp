#include <Arduino.h>
#include <stand.h>

#define SLEEP_TIME_SECONDS 10
#define SELFCHECK_TIME_HOURS 2
#define SLEEP_TIME_MICROSECONDS SLEEP_TIME_SECONDS * 1e6
#define SLEEP_TIME_GOAL ((60 * 60 * SELFCHECK_TIME_HOURS) / SLEEP_TIME_SECONDS) // seg * min * horas / seg
#define RTCMEMORYSTART 65
ADC_MODE(ADC_VCC);

float time_delay = 2.0f;
byte saved_sensor_status = 10;
int debounce = 0;
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
  byte status_readed = get_sensor_status(saved_sensor_status);
  if (status_readed == 200)
  {
    ESP.deepSleep(static_cast<uint64_t>((time_delay / 5) * 1e6));
    return;
  }
  else if (status_readed == FALSE_ALARM_CODE || count >= SLEEP_TIME_GOAL)
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

  if (count <= 60)
  {
    save_param("count", String(count + 1));
  }
  else
  {
    save_param("count", "0");
  }
  log("Going to sleep...");
  delay(3);
  Serial.flush();
  ESP.deepSleep(SLEEP_TIME_MICROSECONDS);
}

void loop()
{
}