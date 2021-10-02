#include "connection.h"

WiFiManager* wm = nullptr;

String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm != nullptr && wm->server->hasArg(name)) {
    value = wm->server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("delay"));
  wm = nullptr;
}

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
  wm = &wifi_manager;
  WiFiManagerParameter custom_delay("<label for='delay'>Delay (em segundos)</label><input type='number' id='delay' name='delay' maxlength='10' value='5.0' min='0.6' step='0.2'>");
  wifi_manager.addParameter(&custom_delay);
  wifi_manager.setSaveParamsCallback(saveParamCallback);
  std::vector<const char *> menu = {"wifi","info","sep","exit"};
  wifi_manager.setMenu(menu);
  //reset settings
  wifi_manager.resetSettings();

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