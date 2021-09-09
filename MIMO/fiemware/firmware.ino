#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>

//for LED status
#include <Ticker.h>
Ticker ticker;

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

#define SERVER_PATH "http://dev.a4rsolucoes.com.br/apis/teste.asp?api="
const int buttonPin = 0; // PINO GPIO0
byte buttonState = 0;    // Estado inicial do botão
String clientName = "";  //Prefixo do nome do dispositivo ESP

int LED = LED_BUILTIN;

void tick()
{
  //toggle state
  digitalWrite(LED, !digitalRead(LED)); // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}

void setup()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  Serial.begin(115200);

  //set led pin as output
  pinMode(LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  // wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(LED, LOW);
}

void loop()
{
  byte newButtonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
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
      }
      else
      {
        Serial.printf("Unable to connect\n");
      }
    }
    else
    {
      Serial.printf("Unable to connect\n");
    }

    delay(400);
  }
  else
  {
    Serial.println("Aguardando mudança:");
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(10000);
}