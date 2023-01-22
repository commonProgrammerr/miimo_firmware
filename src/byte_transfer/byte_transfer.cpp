
#include "byte_transfer.h"

byte tx_pin = 0U;
byte rx_pin = 5U;

void write_bit(bool lv)
{
  digitalWrite(tx_pin, lv ? HIGH : LOW);
  delay(__BYTE_DELAY_MS__);
}

byte read_bit()
{
  volatile byte _bit;
  delay(static_cast<unsigned long>(__BYTE_DELAY_MS__ / 2));
  _bit = digitalRead(rx_pin);
  delay(static_cast<unsigned long>(__BYTE_DELAY_MS__ / 2));
  return _bit;
}

void byte_write(byte data)
{
  bool _bits[8];
  for (byte i = 0; i < 8; i++)
    _bits[i] = (data >> i) % 2 != 0;

  write_bit(HIGH);
  for (byte i = 0; i < 8; i++)
    write_bit(_bits[i]);
  write_bit(LOW);
}

void byte_read(byte *data)
{

  while (digitalRead(rx_pin) != HIGH)
    delayMicroseconds(100);

  delay(static_cast<unsigned long>(__BYTE_DELAY_MS__ / 2));

  for (byte i = 0; i < 8; i++)
    (*data) |= (read_bit() << i);
}