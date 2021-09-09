#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>

//for LED status
#include <Ticker.h>
Ticker ticker;

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

#define SERVER_IP "deva4rsolucoes1.websiteseguro.com"
#define SERVER_CREDENCIAIS "{\"email\":\"manoel@cognvox.net\",\"password\":\"admin\"}"
#ifndef STASSID
#define STASSID "escorel"
#define STAPSK "arer3366547"
#endif

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
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED))
  {

    WiFiClient client;
    HTTPClient https;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    https.begin(client, String("https://") + String(SERVER_IP) + String("/apis/teste.asp")); //HTTP
    https.addHeader("Content-Type", "text/html");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = https.POST(SERVER_CREDENCIAIS);
    Serial.println(httpCode);
    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK)
      {
        const String &payload = https.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    }
    else
    {
      Serial.printf("[HTTP] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }

  delay(10000);
}