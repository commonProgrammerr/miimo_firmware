

#ifndef __dump_serial_h
#define __dump_serial_h
#include <Arduino.h>

#include "../../include/pins.h"
#include <inttypes.h>
#include "Stream.h"

#define MAX_BUFFER_SIZE 32
extern volatile byte buffer[MAX_BUFFER_SIZE];
extern volatile byte head;
extern volatile byte tail;

void handle_store_bytes();

#ifdef ESP8266
void IRAM_ATTR handle_serial();
#define setup_rx_interrupt() \
  attachInterrupt(digitalPinToInterrupt(KEY_PIN), handle_serial, RISING);

#define disable_rx_interrupt() \
  detachInterrupt(digitalPinToInterrupt(KEY_PIN));

#elif defined(__AVR_ATtiny85__)
#define setup_rx_interrupt()                                 \
  cli();               /* Disable interrupts during setup */ \
  MCUCR |= 0b00000011; /* watch for rising edge */           \
  GIMSK |= 1 << INT0;  /* enable external interrupt */       \
  SREG |= 0b10000000;  /* global interrupt enable */         \
  sei();               /* last line of setup - enable interrupts after setup */

#define disable_rx_interrupt()                               \
  cli();               /* Disable interrupts during setup */ \
  MCUCR |= 0b00000010; /* watch for falling edge */          \
  GIMSK -= 1 << INT0;  /* enable external interrupt */       \
  SREG -= 0b10000000;  /* global interrupt enable */         \
  sei();               /* last line of setup - enable interrupts after setup */

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