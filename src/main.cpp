#include <Arduino.h>

#define SLEEP_STEP 500 // attiny default time sleeping
#define READY (uint16_t)0X11
#define SLEEP_1 (uint16_t)0X44
#define SLEEP_0 (uint16_t)0X66
#define AWAKE (uint16_t)0X88
#define DEBOUNCE (uint16_t)0XAA

#ifdef ESP8266
#define DISABLE_DEBUG_LOG
#include <stand.h>
#include <SoftwareSerial.h>

// #define ONE_WIRE_BUS 13
// #define DHTPIN 12

#define SLEEP_TIME_SECONDS 10
#define SELFCHECK_TIME_HOURS 2
#define SLEEP_TIME_MICROSECONDS SLEEP_TIME_SECONDS * 1e6
#define SLEEP_TIME_GOAL ((60 * 60 * SELFCHECK_TIME_HOURS) / SLEEP_TIME_SECONDS) // seg * min * horas / seg
#define RTCMEMORYSTART 65
ADC_MODE(ADC_VCC);

float time_delay = 2.0f;
byte saved_sensor_status = 10;
int debounce = 0;
int count = 0;
SoftwareSerial attiny_serial;

volatile unsigned long conversionStartTime = 0;

void handle_wifi_configuration();
void config();
void esp_hard_sleep(uint16_t signal, uint16_t time = 0X00);

void setup()
{
  config();
  log_value("[main]: delay time = ", time_delay);
  log_value("[main]: saved status = ", saved_sensor_status);
  byte status_readed = get_sensor_status(saved_sensor_status, static_cast<long>((time_delay * 1e6) / 500));
  if (status_readed == 200)
  {
    // ESP.deepSleep(static_cast<uint64_t>((time_delay / 5) * 1e6));
    esp_hard_sleep(DEBOUNCE, static_cast<uint16_t>(time_delay));

    // delay(static_cast<uint64_t>((time_delay / 5) * 1e6));
    // ESP.reset();
    return;
  }
  else if (status_readed == FALSE_ALARM_CODE || count >= SLEEP_TIME_GOAL)
  {
    if (update_server(status_readed))
    {
      saved_sensor_status = sensor();
      count = 0;
    }
  }
  else if (status_readed != saved_sensor_status)
  {
    update_server(status_readed);
    saved_sensor_status = status_readed;
    save_param("status", String(saved_sensor_status));
    delay(5);
  }

  if (count <= 60)
  {
    save_param("count", String(count + 1));
  }
  else
  {
    save_param("count", "0");
  }
  logln("Going to sleep...");
  delay(3);
  log_flush();
  // // ESP.deepSleep(SLEEP_TIME_MICROSECONDS);
  esp_hard_sleep(saved_sensor_status ? SLEEP_0 : SLEEP_1);
}

void loop()
{
  attiny_serial.print(F("sensor:"));
  attiny_serial.println(saved_sensor_status);
  attiny_serial.println(F("loop..."));
  delay(1000);
}

void handle_wifi_configuration()
{

  int should_config = get_saved_param("config").toInt();
  if (should_config && wifi_connection_setup())
  {
    save_param("config", String(0));
  }
  else
  {
    for (byte i = 0; gpioRead(RESTORE_PIN) && i <= 20; i++)
    {
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
  delay(100);
  attiny_serial.begin(100, SWSERIAL_8N1, 9, 5U, true, 12);
  attiny_serial.println(F("\n\n\n\n"));

  WiFi.forceSleepBegin();
  delay(1);
  WiFi.forceSleepWake();
  delay(1);
  params_start_FS();
  delay(10);
  log_flush();
  // handle_wifi_configuration();
  saved_sensor_status = static_cast<byte>(get_saved_param("status").toInt());
  time_delay = get_saved_param("delay").toFloat();
  count = get_saved_param("count").toInt();
}

void esp_hard_sleep(uint16_t signal, uint16_t time)
{
  delayMicroseconds(600);
  // log("[attiny]: ");
  // Serial.println(READY, HEX);
  attiny_serial.write(READY);

  // log("[attiny]: ");
  // Serial.println(signal, HEX);
  attiny_serial.write(signal);

  if (time)
  {
    // log("[attiny]: ");
    // Serial.println(time, HEX);
    attiny_serial.write(time);
  }
}
#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"

//*********************************PINOS************************************
#define AvisoAlarme 1   // saida para avisar que ligou o ESP por alarme
#define AWAKE_ESP_PIN 4 // saida para ligar o regulador 3v3 para o ESP
#define SLEEP_PIN 2     // PB2 - ENTRADA: one wire esp...

//*********************************VARIAVEIS************************************
#define sleepCycle 200 // tempo do ciclo em deep sleep em segundos
// #define sleepCycle 7200   // tempo do ciclo em deep sleep em segundos
#define MAX_AWAKE_TIME 30 // tempo maximo acordado em segundos
#define AWAKE_VALUE 300
uint8_t espec_value = HIGH;
// uint8_t last_status = LOW;

#define __sleep_esp__()             \
  digitalWrite(AWAKE_ESP_PIN, LOW); \
  digitalWrite(AvisoAlarme, 0)
#define __reboot_esp__()        \
  digitalWrite(AvisoAlarme, 1); \
  delayMicroseconds(10);        \
  digitalWrite(AvisoAlarme, 0)

#define __awake_esp__(_reboot)       \
  digitalWrite(AWAKE_ESP_PIN, HIGH); \
  if (_reboot)                       \
    __reboot_esp__();

#define get_sensor_status() \
  analogRead(A3) >= AWAKE_VALUE ? HIGH : LOW

void awake_esp(uint8_t status);

static soft_ring_buffer recived_bytes[10];
TinySoftwareSerial esp_serial(recived_bytes, -1, SLEEP_PIN);

void setup()
{

  pinMode(AWAKE_ESP_PIN, OUTPUT);
  pinMode(AvisoAlarme, OUTPUT);

  digitalWrite(AWAKE_ESP_PIN, 0);
  digitalWrite(AvisoAlarme, 0);

  esp_serial.begin(57600);
}

void loop()
{
  // #define is_timeout() \
//   (timerCount == 0 || timerCount == sleepCycle)

  //   timerCount++;
  delay(SLEEP_STEP); // to save power
  uint8_t current_status = get_sensor_status();

  if (current_status == espec_value)
  {
    awake_esp(current_status);
    // timerCount = 0;
  }
  else
  {
    delay(SLEEP_STEP); // to save power
  }
} // end of loop

void awake_esp(uint8_t status)
{

  __awake_esp__(false);
  while (!esp_serial.available() || esp_serial.read() != READY)
    ;

  switch (esp_serial.read())
  {
  case SLEEP_0:
    espec_value = LOW;
    break;
  case SLEEP_1:
    espec_value = HIGH;
    break;
  case DEBOUNCE:
    // uint16_t total_seconds = (uint16_t)(esp_serial.read());
    // __sleep_esp__();
    // for (int i = 0; i < 5; i++)
    // {
    //   delay(static_cast<uint32_t>(total_seconds / 5));
    //   if (i <= 3 && get_sensor_status() != status)
    //     return;
    //   else if (i > 3)
    //   {
    //     /* code */
    //   }
    // }
    break;
  default:
    break;
  }
  __sleep_esp__();
}

#endif