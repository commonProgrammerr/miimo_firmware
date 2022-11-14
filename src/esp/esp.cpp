
#ifdef ESP8266
#include "esp.h"

volatile byte esp_mode = 0x0;

// sleep
void setup()
{
  // setup_mode
  Serial.begin(115200);
  setup_pins();

  WiFi.forceSleepBegin();
  delay(1);
  WiFi.forceSleepWake();
  delay(1);
  params_start_FS();
  log_flush();

  ping_attiny();
  delay(10);
  handle_wifi_configuration(Dump);

  if (digitalRead(14))
  {
    Serial.println("[main]: Preparing to updte status...");
    send_status();
    ping_attiny();
  }
  else
  {
    Serial.println("[main]: Reseting WIFI data...");
    wifi_reset_config();
  }
}

void loop() {}

#endif