/**
   Código para detectar acionamento de bomba 
   Criado em 21.11.2016
   André Ribeiro
   arer76@hotmail.com

*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

/* this can be run with an emulated server on host:
        cd esp8266-core-root-dir
        cd tests/host
        make ../../libraries/ESP8266WebServer/examples/PostServer/PostServer
        bin/PostServer/PostServer
   then put your PC's IP address in SERVER_ADRESSRESS below, port 9080 (instead of default 80):
*/
//#define SERVER_ADRESSRESS "10.0.1.7:9080" // PC address with emulation on host
#define SERVER_IP "deva4rsolucoes1.websiteseguro.com"
#define SERVER_CREDENCIAIS "{\"email\":\"manoel@cognvox.net\",\"password\":\"admin\"}"
#ifndef STASSID
#define STASSID "escorel"
#define STAPSK "arer3366547"
#endif

void setup()
{

  Serial.begin(115200);
  Serial.println("\n\n\n");

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    Serial.print(".");
  }
  Serial.print("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());
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
    https.begin(client, "https://" SERVER_ADRESSRESS "/apis/teste.asp"); //HTTP
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
