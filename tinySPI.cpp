/*-------------------------------------------------------------------------*
 * tinySPI.h - Arduino hardware SPI master library for ATtiny24/44/84,     *
 * ATtiny25/45/85, and Attiny2313/4313.                                    *
 *                                                                         *
 * Original version of tinyISP by Jack Christensen 24Oct2013               *
 *                                                                         *
 * Added support for Attiny24/25, and Attiny2313/4313                      *
 * by Leonardo Miliani 28Nov2014                                           *                          
 *                                                                         *
 * CC BY-SA-NC:                                                            *
 * This work is licensed under the Creative Commons Attribution-           *
 * ShareAlike- Not Commercial 4.0 Unported License. To view a copy of this *
 * license, visit                                                          *
 * http://creativecommons.org/licenses/by-sa/4.0/ or send a                *
 * letter to Creative Commons, 171 Second Street, Suite 300,               *
 * San Francisco, California, 94105, USA.                                  *
 *-------------------------------------------------------------------------*/

#include "tinySPI.h"

  static void begin(); DONE
  inline static void beginTransaction(SPISettings settings)
  static void usingInterrupt(uint8_t interruptNumber); 
  static void notUsingInterrupt(uint8_t interruptNumber);
  inline static void beginTransaction(SPISettings settings)
  inline static uint8_t transfer(uint8_t data) DONE
  inline static uint16_t transfer16(uint16_t data)
  inline static void transfer(void *buf, size_t count)
  inline static void endTransaction(void)
  static void end(); DONE
  //inline static void setBitOrder(uint8_t bitOrder)
  inline static void setDataMode(uint8_t dataMode)
  //inline static void setClockDivider(uint8_t clockDiv)
  inline static void attachInterrupt() { SPCR |= _BV(SPIE); }
  inline static void detachInterrupt() { SPCR &= ~_BV(SPIE); }
tinySPI::tinySPI() 
{
}
 
void tinySPI::begin(void)
{
    USICR &= ~(_BV(USISIE) | _BV(USIOIE) | _BV(USIWM1));
    USICR |= _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
    USI_DDR_PORT |= _BV(USCK_DD_PIN);   //set the USCK pin as output
    USI_DDR_PORT |= _BV(DO_DD_PIN);     //set the DO pin as output
    USI_DDR_PORT &= ~_BV(DI_DD_PIN);    //set the DI pin as input
}

void tinySPI::setDataMode(uint8_t spiDataMode)
{
    if (spiDataMode == SPI_MODE1)
        USICR |= _BV(USICS0);
    else
        USICR &= ~_BV(USICS0);
}

uint8_t tinySPI::transfer(uint8_t spiData)
{
    USIDR = spiData;
    USISR = _BV(USIOIF);                //clear counter and counter overflow interrupt flag
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { //ensure a consistent clock period
        while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
    }
    return USIDR;
}

uint16_t tinySPI::transfer16(uint16_t data) {
 union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
 in.val = data;
 USIDR = in.msb;
 USISR = _BV(USIOIF);                //clear counter and counter overflow interrupt flag
 ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { //ensure a consistent clock period
  while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
 }
 out.msb = USIDR; 
 USIDR = in.lsb;
 USISR = _BV(USIOIF);                //clear counter and counter overflow interrupt flag
 ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { //ensure a consistent clock period
  while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
 }
 out.lsb = USIDR;
 return out.val;
}

uint16_t transfer(void *buf, size_t count) {
 if (count == 0) return;
 uint8_t *p = (uint8_t *)buf;
 USIDR = *p;
 while (--count > 0) {
  uint8_t out = *(p + 1);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { //ensure a consistent clock period
   while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
  }
  uint8_t in = USIDR;
  USIDR = out;
  *p++ = in;
 }
 ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { //ensure a consistent clock period
   while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
  }
 *p = USIDR;
  }


  inline static void transfer(void *buf, size_t count)

void tinySPI::end(void)
{
    USICR &= ~(_BV(USIWM1) | _BV(USIWM0));
}

tinySPI SPI = tinySPI();                //instantiate a tinySPI object
