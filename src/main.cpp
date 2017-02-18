#include <TinyGPS.h>
#include "../lib/RN2483/src/rn2xx3.h"

/* for standby ability */
#include <RTClock.h>
#include <libmaple/pwr.h>
#include <libmaple/scb.h>
/*
Arduino port

Serial -> DEBUG
Serial1 -> GPS
Serial2 -> LoRaWAN
*/

#define DEBUG 1
const int debug_baud = 9600;
const int gps_baud = 9600;

RTClock rt(RTCSEL_LSE); /* RTC register for STM32 */

TinyGPS gps;
rn2xx3 LoRaWAN(Serial1);

static void noop() {}; /* For RTC? */

void STM32_sleep(uint8_t i){
  rt.createAlarm(&noop, rt.getTime() + i); /* set alarm for interval i */

  // Clear PDDS and LPDS bits
  PWR_BASE->CR &= ~PWR_CR_PDDS;
  PWR_BASE->CR &= ~PWR_CR_LPDS;

  // Clear previous wakeup register
  PWR_BASE->CR |= PWR_CR_CWUF;

  // Set standby
  PWR_BASE->CR |= PWR_CR_PDDS;

  PWR_BASE->CR |= PWR_CR_LPDS;
  SCB_BASE->SCR |= SCB_SCR_SLEEPDEEP;
  asm("    wfi");
}

void setup(){
  if(DEBUG) Serial.begin(debug_baud);
  /* Initialise uart communication to GPS module */
  if(DEBUG) Serial.print("Initialising GPS UART...");
  Serial1.begin(gps_baud);
  if(DEBUG) Serial.print("Done\n");

  /* Initialise uart communication to LoRaWAN module */
  if(DEBUG) Serial.print("Initialising LoRaWAN UART...");
  Serial1.begin(9600); //serial port to radio
  Serial1.flush();
  LoRaWAN.autobaud();
  if(DEBUG) Serial.print("Done\n");
}

void loop(){

  if(DEBUG) Serial.println(LoRaWAN.deveui());

  if(DEBUG) Serial.println("Hello World!");

  /* STM32 sleep */
  STM32_sleep(5); /* engange STM32 stanby for 5 seconds */
}
