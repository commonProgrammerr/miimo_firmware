#include <Arduino.h>

#define SLEEP_STEP 500
#define sw_serial_speed 9600

#include <times.h>
#include <pins.h>

#ifdef ESP8266
// #define DISABLE_DEBUG_LOG
#include <stand.h>

SoftwareSerial attiny_serial(RX_PIN, TX_PIN);

void handle_wifi_configuration();
void config();
void esp_hard_sleep();

void setup()
{
  config();
  pinMode(2, OUTPUT);

  attiny_serial.begin(sw_serial_speed, SWSERIAL_8N1, RX_PIN, TX_PIN);
  attiny_serial.enableRx(true);
  log_value("[main]: delay time = ", time_delay);
  log_value("[main]: saved status = ", saved_sensor_status);

  // byte status_readed = sensor();
  // log_value("[main]: current status = ", status_readed);
  // if (status_readed != saved_sensor_status)
  // {
  //   update_server(status_readed);
  //   saved_sensor_status = status_readed;
  //   save_param("status", String(saved_sensor_status));
  //   delay(5);
  // }

  // esp_hard_sleep();
}

byte last;
void loop()
{
  //   attiny_serial.write('A');
  //   if (attiny_serial.available())
  // {
  //   int read = attiny_serial.read();
  //   Serial.write(read);
  //   Serial.print(':');
  //   Serial.print(read);
  //   Serial.print(" -> ");
  //   Serial.write('A');
  //   Serial.print(':');
  //   Serial.println((int)'A');
  // }
}

void handle_wifi_configuration()
{

  int should_config = get_saved_param("config").toInt();
  if (should_config && wifi_connection_setup())
  {
    save_param("config", String(0));
    time_delay = get_saved_param("delay").toFloat();
    // dto.send(SIG_CONFIG, &time_delay);
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
  Serial.setDebugOutput(true);
  bool isValid = (RX_PIN >= 0 && RX_PIN <= 16) && !isFlashInterfacePin(RX_PIN);
  Serial.print("\nRX pin is valid? ");

  Serial.println(isValid ? "Sim!" : "Não");
  // attiny_serial.isValidGPIOpin

  setup_pins();
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
}
#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"
#include "SoftwareSerial.h"

//*********************************PINOS************************************
// #define TX_ESP_PIN PB0 // saida para avisar que ligou o ESP por alarme
#define TX_ESP_PIN PB1 // saida para avisar que ligou o ESP por alarme
#define RX_ESP_PIN PB2
#define AWAKE_ESP_PIN PB4 // saida para ligar o regulador 3v3 para o ESP

#define AWAKE_VALUE 380
#define MAX_AWAKE_TIME 2 // tempo maximo acordado em minutos

SoftwareSerial esp_serial(RX_ESP_PIN, TX_ESP_PIN, true);
// DTO dto(esp_serial);
// soft_ring_buffer *serial_buffer = Serial._rx_buffer;

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

#define get_sensor_status(read) \
  read >= AWAKE_VALUE ? HIGH : LOW

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

  // pinMode(AWAKE_ESP_PIN, OUTPUT);
  // Serial = TinySoftwareSerial(serial_buffer, TX_ESP_PIN, RX_ESP_PIN);
  esp_serial.begin(sw_serial_speed);
  pinMode(TX_ESP_PIN, OUTPUT);
  pinMode(RX_ESP_PIN, INPUT);
  // pinMode(SLEEP_ESP_PIN, INPUT_PULLUP);
  // pinMode(TX_ESP_PIN, OUTPUT);
  // digitalWrite(TX_ESP_PIN, LOW);

  __awake_esp__();
}

byte print_sensor(int value)
{
  esp_serial.print("Sensor value: ");
  esp_serial.print(value);
  esp_serial.print(" - ");
  esp_serial.println(get_sensor_status(value));
  return get_sensor_status(value);
}

void loop()
{
  if (esp_serial.available())
    esp_serial.write(esp_serial.read());
  // int read = sensor_read();
  // current_value = print_sensor(read);

  // if (current_value == espec_value)
  // {
  //   esp_serial.println("Debounce...");
  //   snore(seconds_ms(5U));
  //   read = sensor_read();
  //   current_value = print_sensor(read);
  // }

  // if (current_value == espec_value)
  // {
  //   esp_serial.println("AWAKE");
  //   espec_value = espec_value == HIGH ? LOW : HIGH;
  // }
  // esp_serial.write((uint8_t)0b01010101);
  // delay(100);
  // delayMicroseconds(10);
}
#endif