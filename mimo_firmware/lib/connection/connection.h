#ifndef _CONNECTION_H
  #define _CONNECTION_H

  #include <Arduino.h>
  #include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

String macToStr(const uint8_t *mac);
String getDeviceName();

//gets called when WiFiManager enters configuration mode
void wifi_config_mode_callback(WiFiManager *myWiFiManager);

bool wifi_connect(WiFiManager &wifi_manager);

bool wifi_reconnect();

void wifi_connection_setup();

#endif