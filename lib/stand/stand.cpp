
#include <stand.h>

byte get_sensor_status(float time, byte last_status)
{

  byte read[] = {10, 10, 10};
  read[0] = sensor();
  log_value("[status]: sensor_read[0] = ", read[0]);

  if (read[0] == last_status)
    return read[0];

  read[1] = get_debounced_status(time * (160 + ((read[0] == 0) ? 200 : 700)));
  log_value("[status]: sensor_read[1] = ", read[1]);

  if (read[0] != 0)
  {
    read[2] = get_debounced_status(time * 140);
    log_value("[status]: sensor_read[2] = ", read[2]);

    bool __ = read[1] == read[0] && read[2] != read[0];

    return __ ? FALSE_ALARM_CODE : read[2];
  }

  return read[1];
}

byte get_debounced_status(float time)
{
  await(time);
  return sensor();
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

void led_on()
{
  // GPIO_OUTPUT_SET(GPIO_ID_PIN(RESTORE_PIN), LOW);
}

void led_off()
{
  // GPIO_OUTPUT_SET(GPIO_ID_PIN(RESTORE_PIN), HIGH);
}