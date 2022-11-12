#include "dump_serial.h"

volatile byte buffer[MAX_BUFFER_SIZE];
volatile byte head = 0;
volatile byte tail = 0;
volatile bool hold = false;

void handle_store_bytes()
{
  hold = true;
  byte _head = head;
  byte i = (_head + 1) % MAX_BUFFER_SIZE;

  if (i != tail)
  {
    buffer[_head] = 0U;
    head = i;
    delayMicroseconds(15);
    for (volatile byte _bit = 0; _bit < 8; _bit++)
    {
      delayMicroseconds(100);
      buffer[_head] |= !digitalRead(RX_PIN) << _bit;
      delayMicroseconds(100);
    }
    hold = false;
  }
  else
  {
    hold = false;
    for (i = 0; i < 8; i++)
      delayMicroseconds(200);
  }
}

void DumpSerial::begin()
{
  pinMode(TX_PIN, OUTPUT);
  pinMode(RX_PIN, INPUT_PULLUP);
  setup_rx_interrupt();

  head = 0;
  tail = 0;
}

size_t DumpSerial::write(byte value)
{
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(200);
  digitalWrite(TX_PIN, LOW);
  delayMicroseconds(30);
  for (byte i = 0; i < 8; i++)
  {
    digitalWrite(TX_PIN, ((value >> i) % 2));
    delayMicroseconds(200);
  }

  return 1;
}

DumpSerial::operator bool()
{
  return true;
}

void DumpSerial::flush()
{
  head = 0;
  tail = 0;
}

void DumpSerial::end()
{
  disable_rx_interrupt();
  head = tail;
}

int DumpSerial::available(void)
{
  return (unsigned int)(MAX_BUFFER_SIZE + head - tail) % MAX_BUFFER_SIZE;
}

int DumpSerial::peek(void)
{
  if (head == tail)
  {
    return -1;
  }
  else
  {
    return buffer[tail];
  }
}

int DumpSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (head == tail)
  {
    return -1;
  }
  else
  {
    byte _tail = tail;
    tail = (_tail + 1) % MAX_BUFFER_SIZE;
    return buffer[_tail];
  }
}

#ifdef __AVR_ATtiny85__
ISR(INT0_vect)
{
  while (hold)
    delayMicroseconds(1);
  handle_store_bytes();
}
#endif

#ifdef ESP8266
void IRAM_ATTR handle_serial()
{
  handle_store_bytes();
}

#endif

DumpSerial Dump;