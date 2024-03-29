#ifndef _STATUS_SERVER_H
#define _STATUS_SERVER_H

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServerSecure.h>

#ifndef SERVER_URL
#define SERVER_URL "https://socketio-teste.herokuapp.com/"
// #define SERVER_URL "http://192.168.15.31:3030/"
// #define SERVER_URL "http://miimo.a4rsolucoes.com.br/apis/registro/"
#define ID_PARAM "API"
#define VALUE_PARAM "VALOR"
#endif

bool check_http_response(HTTPClient &http);
bool update_status_on_server(byte status, String clientID);
#endif