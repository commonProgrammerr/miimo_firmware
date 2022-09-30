#include <Arduino.h>

#define SLEEP_STEP 500 // attiny default time sleeping
#define READY 0X11
#define SLEEP_1 0X44
#define SLEEP_0 0X66
#define AWAKE 0X88
#define DEBOUNCE 0XAA

#ifdef ESP8266
#include <stand.h>
// #include <OneWireSlave.h>
#include "onewire.h"
// #include <SoftwareSerial.h>

#define SLEEP_PIN 5U   // SAIDA: One wire attiny
#define ADC_SIWTCH 16U // SAIDA: Comando para ler bateria
// #define ALARM_PIN 14U
#define LED_PIN 4U

// #define ONE_WIRE_BUS 13
// #define DHTPIN 12

#define SLEEP_TIME_SECONDS 10
#define SELFCHECK_TIME_HOURS 2
#define SLEEP_TIME_MICROSECONDS SLEEP_TIME_SECONDS * 1e6
#define SLEEP_TIME_GOAL ((60 * 60 * SELFCHECK_TIME_HOURS) / SLEEP_TIME_SECONDS) // seg * min * horas / seg
#define RTCMEMORYSTART 65
ADC_MODE(ADC_VCC);

// Pin oneWireData(SLEEP_PIN);

// const byte owROM[7] = {0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};

// This sample emulates a DS18B20 device (temperature sensor), so we start by defining the available commands
// const byte DS18B20_START_CONVERSION = 0x44;
// const byte DS18B20_READ_SCRATCHPAD = 0xBE;
// const byte DS18B20_WRITE_SCRATCHPAD = 0x4E;

float time_delay = 2.0f;
byte saved_sensor_status = 10;
int debounce = 0;
int count = 0;

// enum DeviceState
// {
//   DeviceState::WaitingReset,
//   DeviceState::WaitingCommand,
//   DeviceState::ConvertingTemperature,
//   DeviceState::TemperatureConverted,
// };
// volatile DeviceState state = DeviceState::WaitingReset;

// void owReceive(OneWireSlave::ReceiveEvent evt, byte data);
// void __onewire__();
// scratchpad, with the CRC byte at the end
// volatile byte scratchpad[9];

volatile unsigned long conversionStartTime = 0;

void handle_wifi_configuration();
void config();
void esp_hard_sleep(uint8_t signal, uint8_t time = 0X00);

void setup()
{
  config();
  // OWSlave.setReceiveCallback(&owReceive);
  // OWSlave.begin(owROM, oneWireData.getPinNumber());
  // __onewire__();

  /* ---- MAIN CODE ---- */

  log_value("[main]: delay time = ", time_delay);
  log_value("[main]: saved status = ", saved_sensor_status);
  byte status_readed = get_sensor_status(saved_sensor_status, static_cast<long>((time_delay * 1e6) / 500));
  if (status_readed == 200)
  {
    // ESP.deepSleep(static_cast<uint64_t>((time_delay / 5) * 1e6));
    esp_hard_sleep(DEBOUNCE, static_cast<uint8_t>(time_delay / 5));
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
  log("Going to sleep...");
  delay(3);
  Serial.flush();
  // ESP.deepSleep(SLEEP_TIME_MICROSECONDS);
  esp_hard_sleep(saved_sensor_status ? SLEEP_0 : SLEEP_1);
}

void loop()
{
}

// void __onewire__()
// {
//   delay(10);

//   cli(); // disable interrupts
//   // Be sure to not block interrupts for too long, OneWire timing is very tight for some operations. 1 or 2 microseconds (yes, microseconds, not milliseconds) can be too much depending on your master controller, but then it's equally unlikely that you block exactly at the moment where it matters.
//   // This can be mitigated by using error checking and retry in your high-level communication protocol. A good thing to do anyway.
//   DeviceState localState = state;
//   unsigned long localConversionStartTime = conversionStartTime;
//   sei(); // enable interrupts
//   if (localState == DeviceState::ConvertingTemperature && millis() > localConversionStartTime + 750)
//   {
//     float temperature = 42.0f; // here you could plug any logic you want to return the emulated temperature
//     int16_t raw = (int16_t)(temperature * 16.0f + 0.5f);

//     byte data[9];
//     data[0] = (byte)raw;
//     data[1] = (byte)(raw >> 8);
//     for (int i = 2; i < 8; ++i)
//       data[i] = 0;
//     data[8] = OWSlave.crc8(data, 8);

//     cli();
//     memcpy((void *)scratchpad, data, 9);
//     state = DeviceState::TemperatureConverted;
//     OWSlave.beginWriteBit(1, true); // now that conversion is finished, start sending ones until reset
//     sei();
//   }
// }

// void owReceive(OneWireSlave::ReceiveEvent evt, byte data)
// {
//   switch (evt)
//   {
//   case OneWireSlave::RE_Byte:
//     switch (state)
//     {
//     case DeviceState::WaitingCommand:
//       switch (data)
//       {
//       case DS18B20_START_CONVERSION:
//         state = DeviceState::ConvertingTemperature;
//         conversionStartTime = millis();
//         OWSlave.beginWriteBit(0, true); // send zeros as long as the conversion is not finished
//         break;

//       case DS18B20_READ_SCRATCHPAD:
//         state = DeviceState::WaitingReset;
//         OWSlave.beginWrite((const byte *)scratchpad, 9, 0);
//         break;

//       case DS18B20_WRITE_SCRATCHPAD:

//         break;
//       }
//       break;
//     }
//     break;

//   case OneWireSlave::RE_Reset:
//     state = DeviceState::WaitingCommand;
//     break;

//   case OneWireSlave::RE_Error:
//     state = DeviceState::WaitingReset;
//     break;
//   }
// }

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
  gpio_init();
  // pinMode(SLEEP_PIN, OUTPUT);
  // digitalWrite(SLEEP_PIN, LOW);
  WiFi.forceSleepBegin();
  delay(1);
  WiFi.forceSleepWake();
  delay(1);
  params_start_FS();
  delay(10);
  Serial.flush();
  handle_wifi_configuration();
  saved_sensor_status = static_cast<byte>(get_saved_param("status").toInt());
  time_delay = get_saved_param("delay").toFloat();
  count = get_saved_param("count").toInt();
}

void esp_hard_sleep(uint8_t signal, uint8_t time)
{
  // digitalWrite(SLEEP_PIN, HIGH);
  // delay(1);
  // ESP.deepSleepMax();
  onewire_write(SLEEP_PIN, READY);
  onewire_write(SLEEP_PIN, signal);
  time &&onewire_write(SLEEP_PIN, time);
}
#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"
#include "onewire.h"

//*********************************PINOS************************************
#define AvisoAlarme 1   // saida para avisar que ligou o ESP por alarme
#define AWAKE_ESP_PIN 4 // saida para ligar o regulador 3v3 para o ESP
#define SLEEP_PIN 2     // ENTRADA: one wire esp...

//*********************************VARIAVEIS************************************
#define sleepCycle 10 // tempo do ciclo em deep sleep em segundos
// #define sleepCycle 7200   // tempo do ciclo em deep sleep em segundos
#define MAX_AWAKE_TIME 30 // tempo maximo acordado em segundos
#define AWAKE_VALUE 300
uint16_t timerCount = 0; // variavel para a contagem do tempo
uint16_t last_value = 0; // variavel para armazenar a medida ADC
uint8_t espec_value = HIGH;
// void turnSwitchOn(uint8_t seconds = MAX_AWAKE_TIME);

#define __sleep_esp__()             \
  digitalWrite(AWAKE_ESP_PIN, LOW); \
  digitalWrite(AvisoAlarme, 0)
#define __awake_esp__(_delay)       \
  digitalWrite(AWAKE_ESP_PIN, LOW); \
  delay(_delay);                    \
  digitalWrite(AvisoAlarme, 0)

void turnSwitchOn(uint8_t seconds);

const byte steps = 255;
uint8_t recived_bytes[3];

// enum DeviceState
// {
//   WaitingReset,
//   WaitingCommand,
//   WaitingNewPosition,
//   WaitingNewPositionRelease,
//   WaitingNewControlRegister,
//   WaitingNewControlRegisterRelease
// };

// volatile DeviceState state = DeviceState::WaitingReset;

// This function will be called each time the OneWire library has an event to notify (reset, error, byte received)

void setup()
{
  // Pin mode
  pinMode(AWAKE_ESP_PIN, OUTPUT);
  digitalWrite(AWAKE_ESP_PIN, 1);
  pinMode(AvisoAlarme, OUTPUT);
  digitalWrite(AvisoAlarme, 0);

  // pinMode(SLEEP_PIN, INPUT_PULLUP);
  // while (digitalRead(SLEEP_PIN) != LOW)
  //   ;
}

void loop()
{

  timerCount++;
  delay(SLEEP_STEP); // to save power
  last_value = analogRead(A3);

  // if (lastValue >= AWAKE_VALUE)
  // {
  //   digitalWrite(AWAKE_ESP_PIN, HIGH);
  //   delay(100);
  //   digitalWrite(AWAKE_ESP_PIN, LOW);
  //   delay(100);
  //   digitalWrite(AWAKE_ESP_PIN, HIGH);
  //   delay(100);
  //   digitalWrite(AWAKE_ESP_PIN, LOW);
  //   delay(1000);
  // }
  // else
  // {
  //   digitalWrite(AWAKE_ESP_PIN, HIGH);
  //   delay(2000);
  //   digitalWrite(AWAKE_ESP_PIN, LOW);
  // }

  timerCount++;
  snore(500); // to save power
  last_value = analogRead(A3);

  bool should_awake = last_value > 300 ? espec_value == HIGH : espec_value == LOW;

  if (should_awake || timerCount == 0 || timerCount == sleepCycle)
  {
    turnSwitchOn(MAX_AWAKE_TIME);
    timerCount = 0;
  }
  else
  {
    snore(500); // to save power
  }
} // end of loop

void await_command()
{
  onewire_reset(SLEEP_PIN);

  while (recived_bytes[0] != READY)
    recived_bytes[0] = onewire_read(SLEEP_PIN);

  recived_bytes[1] = onewire_read(SLEEP_PIN);
  recived_bytes[2] = onewire_read(SLEEP_PIN);

  switch (recived_bytes[1])
  {
  case SLEEP_0:
    espec_value = LOW;
    break;
  case SLEEP_1:
    espec_value = HIGH;
    break;
  case DEBOUNCE:
    __sleep_esp__();
    snore(static_cast<uint32_t>(recived_bytes[2] * 1000));
    turnSwitchOn(MAX_AWAKE_TIME);
    break;
  default:
    break;
  }
}

void turnSwitchOn(uint8_t seconds)
{

  uint8_t catchUpTime = 3;
  digitalWrite(AWAKE_ESP_PIN, HIGH);
  // __awake_esp__();
  // delay(catchUpTime * 1000);
  seconds -= catchUpTime;
  await_command();
  // unsigned long started = millis();
  // while (digitalRead(SLEEP_PIN) == HIGH && millis() - started <= seconds * 1000)
  // {

  //   if (lastValue > 300)
  //   {
  //     digitalWrite(AvisoAlarme, 1); // será acionado somente 3 segundos (catchUpTime) depois do esp acordar
  //   }
  // }

  // digitalWrite(AWAKE_ESP_PIN, LOW);
  // digitalWrite(AvisoAlarme, 0);
  __sleep_esp__();
}

#endif