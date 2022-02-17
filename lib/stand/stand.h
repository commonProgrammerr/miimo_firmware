#ifndef __STAND_H
#define __STAND_H

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <rtc_memory.h>
#include "status_server.h"
#include "connection.h"
extern "C"
{
#include "gpio.h"
#include "user_interface.h"
}


#define SENSOR_PIN 0
#define RESTORE_PIN 2
#define CLEAN_CODE 0
#define ALARM_CODE 1
#define FALSE_ALARM_CODE 3

#ifdef DEBUG_LOG
#define log_value(msg, data) (void)0
#define log(msg) (void)0
#else
#define log(msg) (Serial.println(msg))
#define log_value(msg, data) ((Serial.print((msg))), (log((data))), (void)0)
#endif

#define SAVE_SENSOR_STATUS(status) system_rtc_mem_write(64, &status, 4);
#define READ_LAST_STATUS(status) system_rtc_mem_read(64, &status, 4);


#define gpioRead(pin) (GPIO_INPUT_GET(GPIO_ID_PIN(pin)))
#define sensor() (gpioRead(SENSOR_PIN))
#define await(t) (delay(static_cast<long>(t)))

byte get_sensor_status(float time, byte last_status);
byte get_debounced_status(float time);
bool update_server(byte code);
void led_on();
void led_off();

#endif
