#include <TinyGPS.h>

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

void setup(){
  if(DEBUG) Serial.begin(debug_baud);
  /* Initialise uart communication to GPS module */
  if(DEBUG) Serial.print("Initialising GPS UART...");
  Serial1.begin(gps_baud);
  if(DEBUG) Serial.print("Done\n");

}
void loop(){
  if(DEBUG) Serial.println("Hello World!");
}
