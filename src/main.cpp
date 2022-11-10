#include <Arduino.h>

#define SLEEP_STEP 500
#define sw_serial_speed 1

#include <times.h>
#include <pins.h>
#ifdef ESP8266
#include "stand.h"

void IRAM_ATTR handle_configure();

// max value 2410
void write(byte value)
{
  digitalWrite(TX_PIN, HIGH);
  delay(10);
  digitalWrite(TX_PIN, LOW);
  delayMicroseconds(30);
  for (byte i = 0; i < 8; i++)
  {
    digitalWrite(TX_PIN, ((value >> i) % 2));
    delayMicroseconds(200);
  }
  digitalWrite(TX_PIN, LOW);

  Serial.print("0b");
  for (int i = 7; i >= 0; i--)
  {
    Serial.print(((value >> i) % 2));
  }
  Serial.println();
}

void setup()
{
  pinMode(14, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), handle_configure, CHANGE);
  pinMode(TX_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  yield();
}

volatile int interrupts = 0;
void loop()
{
}

#define SETUP_MODE ((digitalRead(14) == LOW))
#define RESET_MODE ((digitalRead(14) == HIGH))
void IRAM_ATTR handle_configure()
{
  interrupts++;
  if (SETUP_MODE)
  {
    Serial.println("\nSetup mode active:");
    Serial.print("  - send timer debounce: ");
    write(50);
    write(60);
    write(40);
    delay(10);
  }
  else if (RESET_MODE)
  {
    Serial.println("\nReset mode active:");
    Serial.print("  - send timer debounce: ");
  }
}

#endif

#ifdef __AVR_ATtiny85__
#include "tinysnore.h"
#include "avr/interrupt.h"
#include "avr/io.h"

//*********************************PINOS************************************
#define TX_ESP_PIN PB1
#define RX_ESP_PIN PB2
#define AWAKE_ESP_PIN PB4

// #define PCINT_VECTOR PCINT0_vect

// #define pinIntterrupt(pin)                                                                                        \
  cli();                      /* Disable interrupts during setup */                                               \
  PCMSK |= (1 << pin);        /*Enable interrupt handler (ISR) for our chosen interrupt pin (PCINT2/PB2/pin 7) */ \
  GIMSK |= (1 << PCIE);       /*Enable PCINT interrupt in the general interrupt mask */                           \
  pinMode(pin, INPUT_PULLUP); /*Set our interrupt pin as input with a pullup to keep it stable */                 \
  sei();                      /*last line of setup - enable interrupts after setup */

#define pinIntterrupt()                                      \
  cli();               /* Disable interrupts during setup */ \
  MCUCR |= 0b00000010; /* watch for falling edge */          \
  GIMSK |= 1 << INT0;  /* enable external interrupt */       \
  SREG |= 0b10000000;  /* global interrupt enable */         \
  sei();               /* last line of setup - enable interrupts after setup */

#define __sleep_esp__() \
  digitalWrite(AWAKE_ESP_PIN, LOW)

void blink(byte times);
int read();

volatile byte _bit_index;
volatile byte buffer[8];
volatile byte size;

#define SETUP_MODE 2
void __awake_esp__(uint8_t mode = 0U)
{

  if (mode > 0)
    digitalWrite(PB1, mode % 2);

  digitalWrite(PB0, LOW);
  digitalWrite(AWAKE_ESP_PIN, HIGH);
  delay(120);
  if (mode > 1)
    digitalWrite(PB0, !digitalRead(PB0));
}

void blink(byte times = 1)
{
  for (byte i = 0; i < times; i++)
  {
    digitalWrite(PB1, !digitalRead(PB1));
    delay(400);
    digitalWrite(PB1, !digitalRead(PB1));
    delay(400);
  }
}

bool debounce()
{
  if (analogRead(A3) < 800)
    return false;
  delay(3000);
  return analogRead(A3) >= 800;
}

int read()
{
  if (!size)
    return -1;

  byte poped_value = buffer[0];

  for (byte i = 1; i < size; i++)
    buffer[i - 1] = buffer[i];

  size--;
  return poped_value;
}

void setup()
{
  pinMode(AWAKE_ESP_PIN, OUTPUT);
  pinMode(PB0, OUTPUT);
  pinMode(PB1, OUTPUT);
  pinMode(PB2, INPUT_PULLUP);
  pinIntterrupt();
  digitalWrite(PB1, 1);

  size = 0;
  _bit_index = 0;
}

void loop()
{
  if (debounce())
  {
    __awake_esp__(2);
    digitalWrite(PB0, !digitalRead(PB0));
  }

  if (size)
  {
    int blinks = read();
    if (blinks > 0)
    {
      delay(2000);
      blink(blinks / 10);
      __sleep_esp__();
    }
  }
}

ISR(INT0_vect)
{
  _bit_index = 0;
  while (digitalRead(PB2) == LOW && _bit_index <= 100)
    delayMicroseconds(10);

  if (_bit_index > 100)
    return;

  if (size >= 8)
    for (_bit_index = 0; _bit_index < 8; _bit_index++)
      delayMicroseconds(200);
  else
  {
    for (_bit_index = 0; _bit_index < 8; _bit_index++)
    {
      delayMicroseconds(120);
      buffer[size] |= !digitalRead(PB2) << _bit_index;
      delayMicroseconds(80);
    }
    size++;
  }
}
#endif