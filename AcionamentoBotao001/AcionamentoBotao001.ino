#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define TOKEN "YOUR THING TOKEN"
#define SERVER_IP "dev.a4rsolucoes.com.br"
const int buttonPin = 0;     // PINO GPIO0
byte buttonState = 0;         // Estado inicial do botão
String clientName = "";      //Prefixo do nome do dispositivo ESP

#define SSID      "escorel"
#define PASS_SSID "arer3366547"

//Trata endereçoo MAC
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

WiFiClient client;
ESP8266WiFiMulti WiFiMulti;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(buttonPin,INPUT_PULLUP);
    
    Serial.begin(115200);
    
    for (uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
    }
    
    //Trata endereço MAC
    uint8_t mac[6];
    WiFi.macAddress(mac);
    clientName += macToStr(mac);
    clientName.replace(":", "");
    
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(SSID, PASS_SSID);
    
}

void loop() {

    /*
    //checkConnection();
    String string;
    while (client.available()) {
        string.concat((char)client.read());
    }
    
    string.toUpperCase();
    string.replace(" ", "");
    */
    // read the state of the pushbutton value:
    byte newButtonState = digitalRead(buttonPin);
    
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState != newButtonState) {
      // turn LED on:
      //digitalWrite(ledPin, HIGH);
      buttonState = newButtonState;
      Serial.println("CONTATO.");
      digitalWrite(LED_BUILTIN, HIGH);    

      if ((WiFiMulti.run() == WL_CONNECTED)) {
      
          WiFiClient client;
      
          HTTPClient http;

          Serial.println("");
          Serial.println("Dispositivo: " + clientName);
      
          Serial.print("Verificando conexão...\n");
          
          if (http.begin(client, "http://dev.a4rsolucoes.com.br/apis/teste.asp?api="+String(buttonState)+"&valor="+clientName)) {  // HTTP
            
            Serial.print("Aguardando retorno...\n");
            // start connection and send HTTP header
            int httpCode = http.GET();
      
            // httpCode will be negative on error
            if (httpCode > 0) {
              // HTTP header has been send and Server response header has been handled
              Serial.printf("Retorno... code: %d\n", httpCode);
      
              // file found at server
              if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                String payload = http.getString();
                Serial.println(payload);
              }
            } else {
              Serial.printf("Falha no envio, error: %s\n", http.errorToString(httpCode).c_str());
            }
      
            http.end();
          } else {
            Serial.printf("Unable to connect\n");
          }
        }
      
        delay(400);      
      
    } else {
      // turn LED off:
      //digitalWrite(ledPin, LOW);
      Serial.println("Aguardando mudança:");
      digitalWrite(LED_BUILTIN, LOW);
      
    } 
    //delay(2000);
    delay(10000);
}
