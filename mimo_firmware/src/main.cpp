#include <Arduino.h>

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>
#include "led_control.h"

extern "C"
{
#include <gpio.h>
#include <user_interface.h>
}




#define SERVER_PATH "http://dev.a4rsolucoes.com.br/apis/teste.asp"
#define BUTTON_PIN 0 // PINO GPIO0

byte buttonState = 0;        // Estado inicial do botão
String clientName = "MIMO_"; //Prefixo do nome do dispositivo ESP


String macToStr(const uint8_t *mac)
{
  String result;
  for (int i = 0; i < 6; ++i)
  {
    result += mac[i] <= 9 ? "0" + String(mac[i], 16) : String(mac[i], 16);
  }
  return result;
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
  delay(100);
  Serial.println("Modo de configuraçao iniciado");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  // ticker.attach(0.2, tick);
}

bool connectWifi(WiFiManager &wifi_manager)
{

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifi_manager.setConfigPortalTimeout(500);
  wifi_manager.setAPCallback(configModeCallback);

  uint8_t mac[6];
  WiFi.macAddress(mac);
  if (clientName.length() < 6)
    clientName += macToStr(mac);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  return wifi_manager.autoConnect(clientName.c_str());
}

bool reconnectWifi()
{
  WiFiManager wifi_manager;
  return connectWifi(wifi_manager);
}

void setupWifi()
{
  led_OFF();
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // ticker.attach(0.2, tick);

  //WiFiManager
  WiFiManager wifi_manager; //Local intialization. Once its business is done, there is no need to keep it around

  //reset settings
  // wifi_manager.resetSettings();

  if (!connectWifi(wifi_manager))
  {
    Serial.println("falha ao conncetar, tempo maximo atingido!");
    //reset and try again
    ESP.restart();
    delay(1000);
  }
  else
  {
    //if you get here you have connected to the WiFi
    Serial.println("conectado!");
    // ticker.detach();

    //if the LED keeps on
  }
}



void setup()
{
  Serial.begin(115200);         // start serial communication
  // pinMode(LED_BUILTIN, OUTPUT); //set led pin as output
  setupWifi();                  // configure wifi connection
}

void sleep()
{
  // led_ON();
  // WiFi.disconnect();
  // wifi_station_disconnect();
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); // set sleep type, the above    posters wifi_set_sleep_type() didnt seem to work for me although it did let me compile and upload with no errors
  // GPIO_ID_PIN(BUTTON_PIN) corresponds to GPIO2 on ESP8266-01 , GPIO_PIN_INTR_LOLEVEL for a logic high, can also do other interrupts, see gpio.h above
  gpio_pin_wakeup_enable(GPIO_ID_PIN(BUTTON_PIN), buttonState ? GPIO_PIN_INTR_LOLEVEL : GPIO_PIN_INTR_HILEVEL);
  wifi_fpm_open(); // Enables force sleep
  delay(1000);
}

void loop()
{
  // sleep();
  // byte newButtonState = digitalRead(BUTTON_PIN);
  // buttonState = digitalRead(BUTTON_PIN);

  led_OFF();
  delay(2000);
  // led_ON();

  // check if the pushbutton is pressed. If the buttonState as diferent of ther last read
  // if (buttonState != newButtonState)
  // {
  //   if (reconnectWifi())
  //   {
  //     Serial.println("CONTATO!");
  //     send_status();
  //   }
  //   else
  //   {
  //     Serial.println("Erro ao connectar com a rede! Reiniciando aparelho");
  //   }
  // }
  // else
  // {
  //   Serial.println("Aguardando mudança de estado...");
  // }
  delay(1000);
}