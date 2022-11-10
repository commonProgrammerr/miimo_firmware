

#ifndef __dump_serial_h
#define __dump_serial_h
#include <Arduino.h>

#include "../../include/pins.h"
#include <inttypes.h>
#include "Stream.h"

#define MAX_BUFFER_SIZE 64
extern volatile byte _bit_index;
extern volatile byte buffer[MAX_BUFFER_SIZE];
extern volatile byte size;
extern volatile byte head;
extern volatile byte tail;

#define new_tail() \
  (tail + 1) % MAX_BUFFER_SIZE

#define new_head() \
  (head + 1) % MAX_BUFFER_SIZE

#ifndef handle_byte_save
#define handle_byte_save()                                \
  _bit_index = 0;                                         \
  while (digitalRead(RX_PIN) == LOW && _bit_index <= 100) \
    delayMicroseconds(10);                                \
                                                          \
  if (_bit_index > 100)                                   \
    return;                                               \
                                                          \
  if (size >= MAX_BUFFER_SIZE)                            \
    for (_bit_index = 0; _bit_index < 8; _bit_index++)    \
      delayMicroseconds(200);                             \
  else                                                    \
  {                                                       \
    tail = new_tail();                                    \
    for (_bit_index = 0; _bit_index < 8; _bit_index++)    \
    {                                                     \
      delayMicroseconds(120);                             \
      buffer[tail] |= !digitalRead(RX_PIN) << _bit_index; \
      delayMicroseconds(80);                              \
    }                                                     \
    size++;                                               \
  }
#endif

#ifdef ESP8266
void IRAM_ATTR handle_serial();
#define setup_rx_interrupt() \
  attachInterrupt(digitalPinToInterrupt(KEY_PIN), handle_serial, RISING);

#define disable_rx_interrupt() \
  detachInterrupt(digitalPinToInterrupt(KEY_PIN));

#endif

class DumpSerial : public Stream
{
public:
  void begin();
  void end();
  virtual int available();
  virtual int peek();
  virtual int read();
  virtual void flush();
  virtual size_t write(uint8_t);
  using Print::write; // pull in write(str) and write(buf, size) from Print
  operator bool();
};

extern DumpSerial Dump;
#endif