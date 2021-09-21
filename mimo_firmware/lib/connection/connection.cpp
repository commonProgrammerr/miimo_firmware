#include "connection.h"

String macToStr(const uint8_t *mac)
{
  String result;
  for (int i = 0; i < 6; ++i)
  {
    result += mac[i] <= 9 ? "0" + String(mac[i], 16) : String(mac[i], 16);
  }
  return result;
}

String getDeviceName()
{
  uint8_t mac[6];
  WiFi.macAddress(mac);
  return "MIIMO_" + macToStr(mac);
}

//gets called when WiFiManager enters configuration mode
void wifi_config_mode_callback(WiFiManager *myWiFiManager)
{
  delay(100);
  Serial.println("Modo de configuraçao iniciado");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

bool wifi_connect(WiFiManager &wifi_manager)
{
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifi_manager.setConfigPortalTimeout(500);
  wifi_manager.setAPCallback(wifi_config_mode_callback);

  
  return wifi_manager.autoConnect(getDeviceName().c_str());
}

bool wifi_reconnect()
{
  WiFiManager wifi_manager;
  return wifi_connect(wifi_manager);
}

void wifi_connection_setup()
{
  WiFi.mode(WIFI_STA);

  WiFiManager wifi_manager;

  //reset settings
  // wifi_manager.resetSettings();

  if (!wifi_connect(wifi_manager))
  {
    Serial.println("falha ao conncetar, tempo maximo atingido!");
    ESP.restart();
    delay(1000);
  }
  else
  {
    Serial.println("conectado!");
  }
}