#include "status_server.h"


bool status_server_http_return_check(HTTPClient &http)
{
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    Serial.printf("Retorno... code: %d\n", httpCode);

    return httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY;
  }
  else
  {
    return false;
  }
}

bool status_server_update_status(byte status, String clientID)
{
  Serial.print("Verificando conexão...\n");
  if ((WiFi.status() == WL_CONNECTED))
  {
    WiFiClient client;
    HTTPClient http;
    String status_query_params = "?api=" + String(status) + "&valor=" + clientID;

    Serial.print("Enviando status... ");
    if (http.begin(client, SERVER_PATH + status_query_params))
    {

      Serial.print("Aguardando retorno...\n");
      if (status_server_http_return_check(http))
      {
        Serial.println(http.getString());
        Serial.flush();
        return true;
      }
      else
      {
        Serial.printf("Falha no envio, error: %s\n", http.errorToString(http.GET()).c_str());
        return false;
      }

      http.end();
    }
    else
    {
      Serial.println("Unable to connect");
      return false;
    }
  }
  else
  {
    Serial.println("Wifi indisponivel");
    return false;
  }
}