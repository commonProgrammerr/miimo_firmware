#ifdef ESP8266
#include "status_server.h"

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
  Serial.print("[HTTP]: Verificando conexão...\n");
  if ((WiFi.status() == WL_CONNECTED))
  {
    // WiFiClient client;
    WiFiClientSecure client;
    HTTPClient http;
    String url_query = (SERVER_URL "?" ID_PARAM "=") + String(client_id) + ("&" VALUE_PARAM "=") + String(status) + "&power=" + String(ESP.getVcc());
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
        Serial.flush();
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