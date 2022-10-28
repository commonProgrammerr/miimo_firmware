#include <Arduino.h>

#define SLEEP_STEP 500
#define sw_serial_speed 9600

#include <times.h>
#include <dto.h>
#include <pins.h>

#ifdef ESP8266
// #define DISABLE_DEBUG_LOG
#include <stand.h>

void handle_wifi_configuration();
void config();
void esp_hard_sleep();

DTO dto(attiny_serial);

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
    time_delay = get_saved_param("delay").toFloat();
    dto.send(SIG_CONFIG, &time_delay);
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
  attiny_serial.begin(sw_serial_speed, SWSERIAL_8N1, RX_PIN, TX_PIN, false, 8);

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
  dto.send(SIG_SLEEP, &saved_sensor_status);
}
#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"
#include "SoftwareSerial.h"

//*********************************PINOS************************************
#define TX_ESP_PIN PB1 // saida para avisar que ligou o ESP por alarme
#define RX_ESP_PIN PB2
#define AWAKE_ESP_PIN PB4 // saida para ligar o regulador 3v3 para o ESP

#define AWAKE_VALUE 380
#define MAX_AWAKE_TIME 2 // tempo maximo acordado em minutos

SoftwareSerial esp_serial(RX_ESP_PIN, TX_ESP_PIN);
DTO dto(esp_serial);

uint8_t espec_value = HIGH;
uint8_t current_value = LOW;
uint64_t time = millis();
float debounce_delay = 15.0;

#define __await(condition) \
  time = millis();         \
  while (!condition && seconds_ms(20U) > (millis() - time))

#define __sleep_esp__() \
  digitalWrite(AWAKE_ESP_PIN, LOW)

#define __awake_esp__() \
  digitalWrite(AWAKE_ESP_PIN, HIGH);

byte get_sleep_status()
{
  byte data;
  byte sig = 0x0;
  while (sig != SIG_SLEEP)
    dto.read(sig, &data);

  return data;
}

#define get_sensor_status() \
  sensor_read() >= AWAKE_VALUE ? HIGH : LOW

int sensor_read()
{
  int read = analogRead(A3);
  delayMicroseconds(420);
  read += analogRead(A3);
  delayMicroseconds(420);
  read += analogRead(A3);

  return static_cast<int>(read / 3);
}

bool debounce_value()
{
  if (current_value == espec_value)
    snore(seconds_ms(15U));
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
  esp_serial.begin(sw_serial_speed);
  // pinMode(SLEEP_ESP_PIN, INPUT_PULLUP);
  pinMode(TX_ESP_PIN, OUTPUT);
  digitalWrite(TX_ESP_PIN, LOW);

  __awake_esp__();

  __await(dto.get_signal() == SIG_READY)
  {
    dto.read();
    if (dto.get_signal() == SIG_CONFIG)
    {
      dto.get_data(&debounce_delay);
      break;
    }
    else
      delayMicroseconds(10);
  }

  __sleep_esp__();
}

void loop()
{

  snore(SLEEP_STEP);
  current_value = get_sensor_status();
  if (debounce_value() || !(millis() % hours_ms(2)))
  {
    __awake_esp__();

    dto.send(SIG_AWAKE, &current_value);
    Data data;
    __await(data.signal == SIG_SLEEP)
    {
      dto.read(&data);
      snore(SLEEP_STEP);
    }

    current_value = data.data[0];

    espec_value = !current_value;

    __sleep_esp__();
  }
}
#endif