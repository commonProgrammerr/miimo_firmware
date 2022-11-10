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
  MCUCR |= 0b00000011; /* watch for rising edge */           \
  GIMSK |= 1 << INT0;  /* enable external interrupt */       \
  SREG |= 0b10000000;  /* global interrupt enable */         \
  sei();               /* last line of setup - enable interrupts after setup */

#define __sleep_esp__() \
  digitalWrite(AWAKE_ESP_PIN, LOW)

void blink(byte times);

#define SETUP_MODE 2
void __awake_esp__(uint8_t mode = 0U)
{
  switch (mode)
  {
  case 1:
    digitalWrite(PB1, 1);
    digitalWrite(PB0, 1);
    break;
  case 2:
    digitalWrite(PB1, 1);
    digitalWrite(PB0, 0);
    break;
  case 3:
    digitalWrite(PB1, 0);
    digitalWrite(PB0, 1);
    break;
  case 4:
    digitalWrite(PB1, 0);
    digitalWrite(PB0, 0);
    break;

  default:
    break;
  }
  byte state = digitalRead(PB0);
  digitalWrite(PB0, LOW);
  digitalWrite(AWAKE_ESP_PIN, HIGH);
  delay(92);
  digitalWrite(PB0, state);
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
  uint32_t time = millis();
  uint8_t read = 0U;

  while (digitalRead(PB2) == HIGH && millis() - time <= 36)
    delayMicroseconds(10);

  while (digitalRead(PB2) == LOW && millis() - time <= 36)
    delayMicroseconds(10);

  if (millis() - time > 36)
    return -1;

  for (byte i = 0; i < 8; i++)
  {
    delayMicroseconds(120);
    read |= !digitalRead(PB2) << i;
    delayMicroseconds(80);
  }

  return read;
}

void setup()
{
  pinMode(AWAKE_ESP_PIN, OUTPUT);
  pinMode(PB0, OUTPUT);
  pinMode(PB1, OUTPUT);
  pinMode(PB2, INPUT_PULLUP);

  // pinIntterrupt(PB2);
  digitalWrite(PB1, 1);
}

void loop()
{
  if (debounce())
  {
    // __awake_esp__(2);

    digitalWrite(PB0, !digitalRead(PB0));
    int blinks = read();
    if (blinks > 0)
    {
      delay(2000);
      blink(blinks / 10);
    }
    else
    {
      digitalWrite(PB1, !digitalRead(PB1));
    }
    delay(600);
    __sleep_esp__();
  }
}

// ISR(INT0_vect)
// {
//   digitalWrite(PB1, !digitalRead(PB2));
// }
#endif