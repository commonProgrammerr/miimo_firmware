#include "params.h"

bool is_file_system_enabled = false;

void save_param(String param_name, String value)
{
  File param_file = LittleFS.open("/" + param_name, "w+");
  if (!param_file)
  {
    Serial.println("[FS Save - " + param_name + "]: " + "Erro ao abrir arquivo!");
  }
  else
  {
    param_file.print(value + "\r");
    Serial.println("[FS Save - " + param_name + "]: " + value);
  }
  param_file.close();
}

String get_saved_param(String param_name)
{
  File param_file = LittleFS.open("/" + param_name, "r");

  if (!param_file)
  {
    Serial.println("[FS Get - " + param_name + "]: " + "Erro ao abrir arquivo!");
    param_file.close();
    return "";
  }

  String param_value = param_file.readStringUntil('\r'); //desconsidera '\r\n'
  Serial.println("[FS Get - " + param_name + "]: " + param_value);
  param_file.close();

  return param_value;
}

void params_start_FS()
{
  is_file_system_enabled = LittleFS.begin();
  if (is_file_system_enabled)
  {
    Serial.println("[FS]: Iniciado com sucesso!");
  }
  else
  {
    Serial.println("[FS]: Erro ao abrir o sistema de arquivos!");
  }
  init_param("delay", "2.0");
  init_param("status", "10");
}

void init_param(String param, String value)
{
  if (!is_file_system_enabled || LittleFS.exists("/" + param))
    return;

  else
  {
    save_param(param, value);
  }
}