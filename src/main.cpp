#include <Arduino.h>
#include <stand.h>

#define RTCMEMORYSTART 65

float time_delay = 2.0f;
byte saved_sensor_status = 10;
// typedef struct {
//     int started;
// } rtc_store;

// rtc_store store;


// void readFromRTCMemory() {
//   system_rtc_mem_read(RTCMEMORYSTART, &store, sizeof(store));
//   yield();
// }

// void writeToRTCMemory() {
//   system_rtc_mem_write(RTCMEMORYSTART, &store, 4);
//   yield();
// }


void config()
{
  Serial.begin(115200);
  gpio_init(); // Initilise GPIO pins
  delay(1000);
  params_start_FS();
  wifi_connection_setup();
  // if(store.started != 0 && ) {
  //   store.started = 0;
  //   readFromRTCMemory();
  // }
  Serial.flush();
  saved_sensor_status = static_cast<byte>(get_saved_param("status").toInt());
  time_delay = get_saved_param("delay").toFloat();
}

void setup()
{
  config();
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
  
  save_param("status", String(saved_sensor_status));
  delay(50);
  log("Going to sleep...");
  delay(3);
  Serial.flush();
  ESP.deepSleep(15e7);
}

void loop()
{}