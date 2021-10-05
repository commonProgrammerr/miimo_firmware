#ifndef PARAMS_H
#define PARAMS_H
  #include <FS.h>
  #include <LittleFS.h>
 
  void params_start_FS();
  void save_param(String param_name, String value);
  String get_saved_param(String param_name);
#endif 