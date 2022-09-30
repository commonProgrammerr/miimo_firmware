#ifndef PARAMS_H
#define PARAMS_H
#ifdef ESP8266
#include <FS.h>
#include <LittleFS.h>

void params_start_FS();
void init_param(String param, String value);
void save_param(String param_name, String value);
String get_saved_param(String param_name);
#endif
#endif