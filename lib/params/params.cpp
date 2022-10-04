#ifdef ESP8266
#include "params.h"

bool is_file_system_enabled = false;

void save_file(String file_name, void *value)
{
  File file = LittleFS.open("/" + file_name, "w+");
  if (!file)
  {
    Serial.println("[FS Save - " + file_name + "]: " + "Erro ao abrir arquivo!");
  }
  else if (sizeof(value) > file.availableForWrite())
    Serial.println("[FS Save - " + file_name + "]: " + "Sem espaço!");
  else
  {
    for (size_t i = sizeof(value); i >= 0; i--)
      file.write(((byte *)value)[i]);
    Serial.println("[FS Save - " + file_name + "]: " + String(sizeof(value)) + "bytes");
  }
  file.close();
}

byte *get_saved_file(String file_name)
{
  File file = LittleFS.open("/" + file_name, "r");

  if (!file)
  {
    Serial.println("[FS Get - " + file_name + "]: " + "Erro ao abrir arquivo!");
    file.close();
    return nullptr;
  }
  char *buff = new char[file.available()];

  Serial.println("[FS Get - " + file_name + "]: " + String(file.readBytes(buff, file.available())) + "bytes");
  file.close();

  return (byte *)buff;
}

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

  String param_value = param_file.readStringUntil('\r'); // desconsidera '\r\n'
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
  uint64_t time = 0;
  init_param("delay", "2.0");
  init_param("status", "10");
  init_param("config", "1");
  init_param("count", "0");
  init_param("time", "0");
  init_param("DEBOUNCE", "0");
}

void init_file(String param, void *value)
{
  if (!is_file_system_enabled || LittleFS.exists("/" + param))
    return;

  else
    save_file(param, value);
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
#endif