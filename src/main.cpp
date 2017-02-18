#include <TinyGPS.h>
#include "../lib/RN2483/src/rn2xx3.h"

/*
Arduino port

Serial -> DEBUG
Serial1 -> GPS
Serial2 -> LoRaWAN
*/

#define DEBUG 1
const int debug_baud = 9600;
const int gps_baud = 9600;

TinyGPS gps;
rn2xx3 LoRaWAN(Serial1);

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
}
