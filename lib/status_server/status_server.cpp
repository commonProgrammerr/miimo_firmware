#ifdef ESP8266
#include "status_server.h"
#include "../logs/logs.h"

#define DC_ADC_Divider 7.6923  // divisor ADC para barramento DC 10k/1k = 10
#define ADC_REF_Voltage 1000.0 // TensÃ£o de alimentacao (mV)

float getBatteryValue()
{
  delay(2);
  digitalWrite(16U, HIGH);
  delay(10);
  int ADCValue = 0; // value read from ADC
  ADCValue += analogRead(A0);
  ADCValue += analogRead(A0);
  ADCValue += analogRead(A0);
  ADCValue += analogRead(A0);
  digitalWrite(16U, LOW);
  float value = (ADCValue / 4) * (DC_ADC_Divider * ADC_REF_Voltage / 1023000UL);
  log_value("[ADC]: Battery current value = ", value);
  return value;
}

bool check_http_response(HTTPClient &http)
{
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    Serial.printf("[HTTP]: Retorno... code: %d\n", httpCode);

    return httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY;
  }
  else
  {
    Serial.printf("[HTTP]: Falha no envio, code: %d\n", httpCode);
    return false;
  }
}

bool update_status_on_server(byte status, String client_id)
{
  log("[HTTP]: Verificando conexão...\n");
  if ((WiFi.status() == WL_CONNECTED))
  {
    // WiFiClient client;
    WiFiClientSecure client;
    HTTPClient http;
    String url_query = (SERVER_URL "?" ID_PARAM "=") + String(client_id) + ("&" VALUE_PARAM "=") + String(status) + "&power=" + String(getBatteryValue());
    client.setInsecure();
    client.connect(url_query, 443);
    // client.connect(url_query, 3030);

    Serial.println("[HTTP]: Enviando status... ");
    bool http_sucess_connection = http.begin(client, url_query);
    if (http_sucess_connection)
    {
      Serial.println("[HTTP]: Aguardando retorno...");
      bool http_sucess_response = check_http_response(http);
      if (http_sucess_response)
      {
        Serial.println("[HTTP]: Return = \"" + http.getString() + "\"");
        log_flush();
      }
      http.end();

      return http_sucess_response;
    }
    else
    {
      Serial.println("[HTTP]: Rede indisponivel");
      return false;
    }
  }
  else
  {
    Serial.println("[HTTP]: Wifi indisponivel");
    return false;
  }
}
#endif