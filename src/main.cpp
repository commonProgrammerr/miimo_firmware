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
#include "tiny_maint.h"

SoftwareSerial esp_serial(RX_PIN, TX_PIN, true);
// DTO dto(esp_serial);
// soft_ring_buffer *serial_buffer = Serial._rx_buffer;

uint8_t espec_value = HIGH;
uint8_t current_value = LOW;
bool configure_mode = false;

uint64_t time = millis();
float debounce_delay = 15.0;

bool debounce_value(uint8_t val = current_value)
{
  if (val == espec_value)
  {
    snore(static_cast<uint32_t>(seconds_ms(debounce_delay)));
    val = get_sensor_status();
  }

  current_value = val;
  return val == espec_value;
}

#define read_time() (esp_serial.available() == sizeof(float) && esp_serial.readBytes((byte *)&debounce_delay, sizeof(float)))

void setup()
{

  esp_serial.begin(sw_serial_speed);
  setup_pins();

  __awake_esp__();
  __await_wake__();

  if (was_timeout)
    return;

  __configure_mode__();
  __await__(read_time())
      delay(100);
}

void loop()
{
  __sleep_esp__();
  snore(seconds_ms(2));

  if (configure_mode || sensor_value() >= CONFIG_VALUE)
  {
    configure_mode = true;
    __awake_esp__();
    __await_wake__();

    if (was_timeout)
      return;

    __reset_mode__();
    __await_wake__();

    if (was_timeout)
      return;

    __configure_mode__();
    __await_timeout__(read_time(), seconds_ms(500))
        delay(100);

    if (was_timeout)
      return;

    configure_mode = false;
  }
  else if (debounce_value(get_sensor_status()))
  {
    __awake_esp__();
    __await_wake__();

    if (was_timeout)
      return;

    __awake_sensor_mode__();
    __await_timeout__(check_signal(SLEEP_SIG), seconds_ms(40));

    if (was_timeout)
      return;

    espec_value = current_value;
  }
}
#endif