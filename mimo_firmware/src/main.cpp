#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>

//for LED status
#include <Ticker.h>
Ticker ticker;

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

#define SERVER_PATH "http://dev.a4rsolucoes.com.br/apis/teste.asp?api="
#define BUTTON_PIN 0 // PINO GPIO0

byte buttonState = 0;        // Estado inicial do botão
String clientName = "MIMO_"; //Prefixo do nome do dispositivo ESP

void tick()
{
  //toggle state
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // set pin to the opposite state
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

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
  delay(100);
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  //set led pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.2, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  //reset settings
  wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setConfigPortalTimeout(180);
  wm.setAPCallback(configModeCallback);

  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect(clientName.c_str()))
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again
    ESP.restart();
    delay(1000);
  }
  else
  {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    ticker.detach();

    //if the LED keeps on
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop()
{
  byte newButtonState = digitalRead(BUTTON_PIN);

  // check if the pushbutton is pressed. If the buttonState as diferent of ther last read:
  if (buttonState != newButtonState)
  {
    // turn LED on:
    //digitalWrite(ledPin, HIGH);
    buttonState = newButtonState;
    Serial.println("CONTATO.");
    digitalWrite(LED_BUILTIN, HIGH);

    if ((WiFi.status() == WL_CONNECTED))
    {

      WiFiClient client;

      HTTPClient http;

      Serial.println("");
      Serial.println("Dispositivo: " + clientName);

      Serial.print("Verificando conexão...\n");

      if (http.begin(client, SERVER_PATH + String(buttonState) + "&valor=" + clientName))
      { // HTTP

        Serial.print("Aguardando retorno...\n");
        ticker.attach(0.8, tick);
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if (httpCode > 0)
        {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("Retorno... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
            String payload = http.getString();
            Serial.println(payload);
          }
        }
        else
        {
          Serial.printf("Falha no envio, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        ticker.detach();
      }
      else
      {
        Serial.println("Unable to connect");
      }
    }
    else
    {
      Serial.println("Unable to connect");
    }

    delay(400);
  }
  else
  {
    Serial.println("Aguardando mudança:");
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(1000);
}