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
    return false;
  }
}

bool update_status_on_server(byte status, String client_id)
{
  Serial.print("[HTTP]: Verificando conexão...\n");
  if ((WiFi.status() == WL_CONNECTED))
  {
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP]: Enviando status... ");
    if (http.begin(client, (SERVER_URL "?" ID_PARAM "=") + String(client_id) + ("&" VALUE_PARAM "=") + String(status)))
    {

      Serial.println("[HTTP]: Aguardando retorno...");
      if (check_http_response(http))
      {
        Serial.println("[HTTP Return]: " + http.getString());
        Serial.flush();
        return true;
      }
      else
      {
        Serial.printf("[HTTP]: Falha no envio, error: %s\n", http.errorToString(http.GET()).c_str());
        return false;
      }

      http.end();
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