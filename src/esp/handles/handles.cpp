
#ifdef ESP8266
#include "handles.h"

volatile float time_delay = 2.0f;
volatile byte saved_sensor_status = 10;

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

void handle_wifi_configuration(Stream &reg)
{
  if (get_saved_param("config").toInt() && wifi_connection_setup())
  {
    save_param("config", "0");
    time_delay = get_saved_param("delay").toFloat();
    reg.write(static_cast<byte>(time_delay * 10));
    Serial.print("Send timer: ");
    Serial.println(time_delay * 10);
  }
}

void ping_attiny(uint32 delay)
{
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(delay);
  digitalWrite(TX_PIN, LOW);
}

void send_status()
{
  time_delay = get_saved_param("delay").toFloat();
  saved_sensor_status = static_cast<byte>(get_saved_param("status").toInt());
  byte status_readed = sensor();
  log_value("[main]: delay time = ", time_delay);
  log_value("[main]: saved status = ", saved_sensor_status);
  log_value("[main]: current status = ", status_readed);
  if (status_readed != saved_sensor_status)
  {
    update_server(status_readed);
    saved_sensor_status = status_readed;
    save_param("status", String(saved_sensor_status));
    delay(5);
  }
}

#endif