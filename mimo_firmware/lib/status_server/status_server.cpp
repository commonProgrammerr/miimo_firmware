#include "status_server.h"


bool check_http_response(HTTPClient &http)
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

bool update_status_on_server(byte status, String clientID)
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
      if (check_http_response(http))
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
      Serial.println("Rede indisponivel");
      return false;
    }
  }
  else
  {
    Serial.println("Wifi indisponivel");
    return false;
  }
}