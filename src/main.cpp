#include <Arduino.h>

#define SLEEP_STEP 500

#include <dto.h>
#include <pins.h>

#ifdef ESP8266
// #define DISABLE_DEBUG_LOG
#include <times.h>
#include <stand.h>

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
    DTO dto;
    time_delay = get_saved_param("delay").toFloat();
    set_dto_data(dto, &time_delay);
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

  DTO _dto;
  _dto.signal = SIG_SLEEP;
  _dto.data[0] = saved_sensor_status;
  _dto.size = 1U;
  send_dto(attiny_serial, _dto);
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

#define sensor_read() \
  static_cast<int>((analogRead(A3) + analogRead(A3) + analogRead(A3)) / 3)

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