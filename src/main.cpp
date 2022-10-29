#include <Arduino.h>

#define SLEEP_STEP 500

#define seconds(s) (s * 1000)
#define u_seconds(s) (s * 1000U)
#define minutes(_minutes) (_minutes * seconds(60))
#define u_minutes(_minutes) (_minutes * u_seconds(60))
#define hours(_hours) (_hours * minutes(60))
#define u_hours(_hours) (_hours * u_minutes(60))
#define days(_days) (_days * hours(24))
#define u_days(_days) (_days * u_hours(24))

#ifdef ESP8266
// #define DISABLE_DEBUG_LOG
#include <stand.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br");

#define SLEEP_TIME_SECONDS 10
#define SELFCHECK_TIME_HOURS 2
#define SLEEP_TIME_MICROSECONDS SLEEP_TIME_SECONDS * 1e6
#define SLEEP_TIME_GOAL ((60 * 60 * SELFCHECK_TIME_HOURS) / SLEEP_TIME_SECONDS) // seg * min * horas / seg

float time_delay = 2.0f;
byte saved_sensor_status = 10;
uint64_t saved_time = 0;

void handle_wifi_configuration();
void config();
void esp_hard_sleep();

void setup()
{
  config();
  log_value("[main]: delay time = ", time_delay);
  log_value("[main]: saved status = ", saved_sensor_status);
  byte status_readed = sensor();
  log_value("[main]: current status = ", status_readed);
  if (status_readed != saved_sensor_status)
  {
    update_server(status_readed);
    saved_sensor_status = status_readed;
    save_param("status", String(saved_sensor_status));
    delay(5);
  }
  // else
  // {
  //   log_value("[time]: saved epoch time = ", saved_time);
  //   logln("[time]: updating time...");
  //   timeClient.forceUpdate();
  //   yield();
  //   logln("[time]: done!");
  //   log_value("[time]: current epoch time = ", timeClient.getEpochTime());
  //   if (timeClient.getEpochTime() - saved_time > (u_hours(2) / 1000))
  //   {
  //     saved_time = timeClient.getEpochTime();
  //     save_param("time", String(saved_time));
  //     update_server(status_readed);
  //   }
  // }

  esp_hard_sleep();
}

void loop()
{
}

void handle_wifi_configuration()
{

  int should_config = get_saved_param("config").toInt();
  if (should_config && wifi_connection_setup())
  {
    save_param("config", String(0));
    timeClient.begin();
    timeClient.update();
  }
  else
  {
    for (byte i = 0; !digitalRead(RESTORE_PIN) && i <= 20; i++)
    {
      Serial.print("Reset pin read: ");
      Serial.println(!digitalRead(RESTORE_PIN));
      if (i == 20)
        wifi_reset_config();

      delay(65);
    }
  }
}

void config()
{
  Serial.begin(115200);
  setup_pins();
  digitalWrite(SLEEP_PIN, LOW);
  delay(100);
  WiFi.forceSleepBegin();
  delay(1);
  WiFi.forceSleepWake();
  delay(1);
  params_start_FS();
  delay(10);
  log_flush();

  handle_wifi_configuration();
  saved_sensor_status = static_cast<byte>(get_saved_param("status").toInt());
  time_delay = get_saved_param("delay").toFloat();
  saved_time = static_cast<uint64_t>(get_saved_param("time").toInt());
}

void esp_hard_sleep()
{
  logln(F("going to sleep..."));
  digitalWrite(SLEEP_PIN, HIGH);
}
#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"

//*********************************PINOS************************************
#define TX_ESP_PIN PB1    // saida para avisar que ligou o ESP por alarme
#define AWAKE_ESP_PIN PB4 // saida para ligar o regulador 3v3 para o ESP
#define SLEEP_ESP_PIN PB2

#define AWAKE_VALUE 380
#define MAX_AWAKE_TIME 2 // tempo maximo acordado em minutos

uint8_t espec_value = HIGH;
uint8_t current_value = LOW;
uint64_t time = millis();

#define await_esp() \
  time = millis();  \
  while (get_sleep_status() == HIGH && u_seconds(20) > (millis() - time))

#define __sleep_esp__() \
  digitalWrite(AWAKE_ESP_PIN, LOW)

#define __awake_esp__() \
  digitalWrite(AWAKE_ESP_PIN, HIGH);

int sensor_read()
{
  int read = analogRead(A3);
  delayMicroseconds(20);
  read += analogRead(A3);
  delayMicroseconds(20);
  read += analogRead(A3);

  return static_cast<int>(read / 3);
}

#define get_sleep_status() \
  digitalRead(SLEEP_ESP_PIN)

#define get_sensor_status() \
  sensor_read() >= AWAKE_VALUE ? HIGH : LOW

#define pass_status() \
  digitalWrite(TX_ESP_PIN, get_sensor_status())

bool debounce_value()
{
  if (current_value == espec_value)
    snore(u_seconds(15));
  // for (uint8_t times = 8; times > 0; times--)
  // {
  //   if (get_sensor_status() != espec_value)
  //     return false;
  //   delay(u_seconds(2));
  // }
  current_value = sensor_read();
  return current_value == espec_value;
}

void setup()
{

  pinMode(AWAKE_ESP_PIN, OUTPUT);
  pinMode(SLEEP_ESP_PIN, INPUT_PULLUP);
  pinMode(TX_ESP_PIN, OUTPUT);
  digitalWrite(TX_ESP_PIN, LOW);

  __awake_esp__();

  while (get_sleep_status() == HIGH)
    snore(100);

  __sleep_esp__();
}

void loop()
{

  snore(SLEEP_STEP);
  current_value = get_sensor_status();
  if (debounce_value() || !(millis() % u_hours(2)))
  {
    __awake_esp__();
    pass_status();
    await_esp()
        snore(SLEEP_STEP);

    espec_value = !current_value;

    __sleep_esp__();
  }
}
#endif