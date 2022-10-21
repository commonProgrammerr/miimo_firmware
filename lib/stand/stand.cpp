
#ifdef ESP8266
#include <stand.h>

// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "a.st1.ntp.br");
float time_delay = 2.0f;
byte saved_sensor_status = 10;
uint64_t saved_time = 0;
SoftwareSerial attiny_serial;

byte get_sensor_status(byte last_status, long max)
{
  byte read = sensor();
  long debounce = get_saved_param("DEBOUNCE").toInt();
  if (read == last_status && debounce == 0)
  {
    return read;
  }

  log_value("[status]: sensor_read[" + String(debounce) + "] = ", read);

  if (read == 0 && debounce <= 3)
  {
    if (debounce != 0)
      save_param("DEBOUNCE", "0");
    return 0;
  }
  else if (read == 0 && debounce >= max)
  {
    if (debounce != 0)
      save_param("DEBOUNCE", "0");
    return FALSE_ALARM_CODE;
  }
  else if (read == 1 && debounce <= max)
  {

    save_param("DEBOUNCE", String(debounce + 1));
    return 200;
  }
  else
  {
    if (debounce != 0)
      save_param("DEBOUNCE", "0");
    return 1;
  }
}

byte get_debounced_status(float time)
{
  await(time);
  return sensor();
}

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

void led_on()
{
  // GPIO_OUTPUT_SET(GPIO_ID_PIN(RESTORE_PIN), LOW);
}

void led_off()
{
  // GPIO_OUTPUT_SET(GPIO_ID_PIN(RESTORE_PIN), HIGH);
}
#endif