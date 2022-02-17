#include <Arduino.h>
#include <stand.h>

float time_delay = 2.0f;
byte saved_sensor_status = 10;
int started;
typedef struct {
    int started;
} RTCSavedData;

void config(boolean config)
{
  Serial.begin(115200);
  gpio_init(); // Initilise GPIO pins
  delay(1000);
  params_start_FS();
  config ? wifi_connection_setup() : false;
  Serial.flush();
  saved_sensor_status = static_cast<byte>(get_saved_param("delay").toInt());
  time_delay = get_saved_param("delay").toFloat();
}

void setup()
{
  RtcMemory* rtcMemory = new RtcMemory();
  rtcMemory->begin();
  RTCSavedData* rtcData = rtcMemory->getData<RTCSavedData>();
  started = rtcData->started;
  config(started == 0);
  
  log_value("[main]: delay time = ", time_delay);
  log_value("[main]: saved status = ", saved_sensor_status);
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
  rtcData->started = 1;
  rtcMemory->save();
  save_param("status", String(saved_sensor_status));
  delay(50);
  log("Going to sleep...");
  delay(3);
  Serial.flush();
  ESP.deepSleep(15e7);
}

void loop()
{}