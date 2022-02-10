#include <Arduino.h>
#include <stand.h>

float time_delay = 2.0f;
byte saved_sensor_status = 10;

void wakeup_callback()
{
  log("[wakeup]: Wake!");
  log_value("[wakeup]: delay time = ", time_delay);
  log_value("[wakeup]: last status = ", saved_sensor_status);
  led_on();
}

void sleep()
{
  led_off();

  do
  {
    if ((WiFi.status() == WL_CONNECTED))
    {
      WiFi.mode(WIFI_OFF);
      WiFi.forceSleepBegin();
      log("[sleep]: WiFi is down");
      delay(3);
    }
    log("[sleep]: Awating sensor status, to be LOW.");
  } while (get_debounced_status(100) != 0);

  wifi_station_disconnect();
  wifi_set_opmode_current(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_set_wakeup_cb(wakeup_callback);

  log("[sleep]: Set wakeup mode to HILEVEL");
  log("[sleep]: Going to sleep now...");
  delay(6);

  // gpio_pin_wakeup_enable(GPIO_ID_PIN(SENSOR_PIN), GPIO_PIN_INTR_HILEVEL);
  wifi_fpm_open();
    // wifi_fpm_do_sleep(0xFFFFFFF);  
    wifi_fpm_do_sleep(0xFFF0000);  
    // wifi_fpm_do_sleep(0xAFFFFFF);
  delay(200);
}

void setup()  
{
  Serial.begin(115200);
  gpio_init(); // Initilise GPIO pins
  delay(1000);
  params_start_FS();
  wifi_connection_setup();
  Serial.flush();
  time_delay = get_saved_param("delay").toFloat();
}

void loop()
{
  byte status_readed = get_sensor_status(time_delay, saved_sensor_status);
  log_value("[main]: final status = ", status_readed);

  if (status_readed == FALSE_ALARM_CODE)
  {
    if (update_server(status_readed))
      saved_sensor_status = sensor();
  }
  else if (status_readed != saved_sensor_status)
  {
    update_server(status_readed);
    saved_sensor_status = status_readed;
  }
  led_off();
  sleep();
  delay(200);
}