#ifndef __byte_transfer_h__
#define __byte_transfer_h__

#include <Arduino.h>

extern byte tx_pin;
extern byte rx_pin;

void byte_write(byte data);
void byte_read(byte *data);

#endif