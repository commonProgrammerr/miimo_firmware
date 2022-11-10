#include "dump_serial.h"

volatile byte _bit_index;
volatile byte buffer[MAX_BUFFER_SIZE];
volatile byte size;
volatile byte head;
volatile byte tail;

void DumpSerial::begin()
{
  pinMode(TX_PIN, OUTPUT);
  pinMode(RX_PIN, INPUT_PULLUP);
  flush();
  setup_rx_interrupt();
}

int DumpSerial::available() { return size; }
void DumpSerial::end() { disable_rx_interrupt(); }

int DumpSerial::peek()
{
  if (head == tail)
    return -1;
  else
    return buffer[head];
}

int DumpSerial::read()
{
  if (size >= MAX_BUFFER_SIZE || head == tail)
    return -1;

  byte poped_value = buffer[head];
  head = new_head();
  size--;

  return poped_value;
}

size_t DumpSerial::write(byte value)
{
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(100);
  digitalWrite(TX_PIN, LOW);
  delayMicroseconds(30);
  for (byte i = 0; i < 8; i++)
  {
    digitalWrite(TX_PIN, ((value >> i) % 2));
    delayMicroseconds(200);
  }
  digitalWrite(TX_PIN, LOW);
  delay(2);

  return 1;
}

void DumpSerial::flush()
{
  head = 0;
  tail = 0;
  size = 0;
}

DumpSerial::operator bool()
{
  return true;
}

#ifdef __AVR_ATtiny85__
ISR(INT0_vect)
{
  handle_byte_save();
}
#endif

#ifdef ESP8266
void IRAM_ATTR handle_serial()
{
  handle_byte_save();
}

#endif

DumpSerial Dump;