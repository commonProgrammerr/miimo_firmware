
#ifdef ESP8266
#include "stand.h"
byte saved_sensor_status = 10;

bool update_server(byte code)
{
  if (WiFi.status() == WL_CONNECTED || wifi_reconnect())
  {
    logln("[main]: Conectado!");
    logln("[main]: Enviando...");
    log_value("[main]: Sensor status: ", code);
    if (update_status_on_server(code, getDeviceName().substring(6)))
    {
      logln("Sucesso!");
      return true;
    }
    else
    {
      logln("Error!");
      return false;
    }
  }
  else
  {
    logln("[WiFi]: Wifi indisponivel!");
    return false;
  }
}

void handle_wifi_configuration()
{

  int should_config = get_saved_param("config").toInt();
  if (should_config && wifi_connection_setup())
  {
    save_param("config", String(0));
  }
  else if (CH1 == LOW)
  {
    Serial.print("Reseting configuration...");
    wifi_reset_config();
    delay(65);
  }
}

void config()
{
  Serial.begin(115200);
  setup_pins();
  WiFi.forceSleepBegin();
  delayMicroseconds(200);
  WiFi.forceSleepWake();
  delayMicroseconds(200);
  params_start_FS();
  delayMicroseconds(200);
  log_flush();

  writeWak(LOW);
  handle_wifi_configuration();
  saved_sensor_status = static_cast<byte>(get_saved_param("status").toInt());
}
#endif