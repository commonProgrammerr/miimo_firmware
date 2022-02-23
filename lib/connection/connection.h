#ifndef _CONNECTION_H
#define _CONNECTION_H

#include "params.h"
#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define DELAY_FIELD_HTML "<label for='delay'>Delay (em segundos)</label><input type='number' id='delay' name='delay' maxlength='10' value='15.0' min='0.6' step='0.2'>"

String macToStr(const uint8_t *mac);
String getDeviceName();

//gets called when WiFiManager enters configuration mode
void wifi_config_mode_callback(WiFiManager *myWiFiManager);
void wifi_reset_config();

bool wifi_connect(WiFiManager &wifi_manager);

bool wifi_reconnect();

bool wifi_connection_setup();

#endif